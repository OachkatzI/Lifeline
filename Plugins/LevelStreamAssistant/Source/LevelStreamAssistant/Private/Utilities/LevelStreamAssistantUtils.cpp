// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Utilities/LevelStreamAssistantUtils.h"
#include "EditorMode/LevelStreamAssistantEdMode.h"
#include "Engine/Level.h"
#include "Engine/LevelStreaming.h"
#include "Engine/StaticMesh.h"
#include "Engine/BrushBuilder.h"
#include "Engine/Polys.h"
#include "GameFramework/Volume.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BrushComponent.h"
#include "Model.h"
#include "BSPOps.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "Editor.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "EditorModeManager.h"
#include "IAssetViewport.h"
#include "EditorViewportClient.h"

#define LOCTEXT_NAMESPACE "LevelStreamAssistantUtils"

namespace LevelStreamAssistantUtils
{
	FLevelStreamAssistantEdMode* GetPluginMode()
	{
		return GLevelEditorModeTools().GetActiveModeTyped<FLevelStreamAssistantEdMode>(FLevelStreamAssistantEdMode::EM_LevelStreamAssistantEdModeId);
	}

	UWorld* GetWorldFromEditor()
	{
		return GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	}

	TArray<ULevelStreaming*> GetStreamingLevelsFromEditorWorld()
	{
		UWorld* World = GetWorldFromEditor();
		return World ? World->GetStreamingLevels() : TArray<ULevelStreaming*>();
	}

	FString GetLevelStreamingName(const ULevelStreaming* LevelStreaming)
	{
		return LevelStreaming ? FPaths::GetCleanFilename(LevelStreaming->GetWorldAssetPackageName()) : TEXT("");
	}

	bool CompareLevelNames(const TWeakObjectPtr<ULevelStreaming>& LevelA, const TWeakObjectPtr<ULevelStreaming>& LevelB, bool bAscending)
	{
		if (!LevelA.IsValid() && !LevelB.IsValid()) return true;
		if (!LevelA.IsValid()) return false;
		if (!LevelB.IsValid()) return true;
		return CompareNames(GetLevelStreamingName(LevelA.Get()), GetLevelStreamingName(LevelB.Get()), bAscending);
	}

	bool CompareActorNames(const TWeakObjectPtr<AActor>& ActorA, const TWeakObjectPtr<AActor>& ActorB, bool bAscending)
	{
		if (!ActorA.IsValid() && !ActorB.IsValid()) return true;
		if (!ActorA.IsValid()) return false;
		if (!ActorB.IsValid()) return true;
		return CompareNames(ActorA->GetActorLabel(), ActorB->GetActorLabel(), bAscending);
	}

	bool CompareNames(const FString& NameA, const FString& NameB, bool bAscending)
	{
		return bAscending == (NameA <= NameB);
	}

	FName GetCommonPath(const TArray<FName>& Paths)
	{
		TArray<FString> StrPaths;
		StrPaths.Reserve(Paths.Num());
		for (const FName& Path : Paths)
		{
			StrPaths.Add(Path != NAME_None ? Path.ToString() : TEXT(""));
		}

		int32 CommonLen = 0;
		bool bFirstItr = true;
		for (const FString& Path : StrPaths)
		{
			if (bFirstItr)
			{
				CommonLen = Path.Len();
				bFirstItr = false;
			}

			if (Path.Len() < CommonLen) CommonLen = Path.Len();
		}

		TFunctionRef<bool(TCHAR)> IsSlashOrBackslash = [](TCHAR C) { return C == TEXT('/') || C == TEXT('\\'); };

		FString CommonPath;
		for (int32 CharIndex = 0 ; CharIndex < CommonLen; CharIndex++)
		{
			check(StrPaths.Num() > 0);
			TCHAR FirstChar = StrPaths[0][CharIndex];
			bool bAllCharIsEqual = true;

			for (int32 PathIndex = 1; PathIndex < StrPaths.Num(); PathIndex++)
			{
				TCHAR OtherChar = StrPaths[PathIndex][CharIndex];

				if (IsSlashOrBackslash(FirstChar) && IsSlashOrBackslash(OtherChar)) continue;

				if (FirstChar != OtherChar)
				{
					bAllCharIsEqual = false;
					break;
				}
			}

			if (bAllCharIsEqual)
			{
				CommonPath.AppendChar(FirstChar);
			}
			else
			{
				CommonPath = FPaths::GetPath(CommonPath);
				break;
			}
		}

		return *CommonPath;
	}

	void LevelToStringArray(TWeakObjectPtr<ULevelStreaming> InLevel, TArray<FString>& Array)
	{
		if (ULevelStreaming* Level = InLevel.Get())
		{
			// include level name with path in the filter.
			Array.Add(Level->GetWorldAssetPackageName());
		}
	}

	void ActorToStringArray(TWeakObjectPtr<AActor> InActor, TArray<FString>& Array)
	{
		if (AActor* Actor = InActor.Get())
		{
			// include both name and label in the filter.
			Array.Add(Actor->GetName());
			Array.Add(Actor->GetActorLabel());
		}
	}

	void FlushLevelStreaming()
	{
		if (UWorld* World = GetWorldFromEditor())
		{
			World->FlushLevelStreaming();
		}
	}

	FBox CalculateLevelBounds(ULevel* InLevel, bool bIgnoreStaticMeshBoundsExtension)
	{
		FBox LevelBounds(ForceInit);

		if (InLevel)
		{
			LevelBounds = CalculateBoundsFromActors(InLevel->Actors, bIgnoreStaticMeshBoundsExtension);
		}

		return LevelBounds;
	}

	FBox CalculateBoundsFromActors(const TArray<AActor*>& Actors, bool bIgnoreStaticMeshBoundsExtension)
	{
		FBox ActorsBounds(ForceInit);

		for (AActor* Actor : Actors)
		{
			if (Actor && Actor->IsLevelBoundsRelevant())
			{
				// Sum up components bounding boxes
				FBox ActorBox = CalculateActorBounds(Actor, bIgnoreStaticMeshBoundsExtension);
				if (ActorBox.IsValid)
				{
					ActorsBounds += ActorBox;
				}
			}
		}

		return ActorsBounds;
	}

	FBox CalculateActorBounds(const AActor* Actor, bool bIgnoreStaticMeshBoundsExtension)
	{
		FBox Box(ForceInit);

		if (Actor)
		{
			const bool bNonColliding = true;
			TInlineComponentArray<UPrimitiveComponent*> PrimitiveComponents(Actor);
			for (const UPrimitiveComponent* PrimComp : PrimitiveComponents)
			{
				// Only use collidable components to find collision bounding box.
				if (PrimComp && (bNonColliding || PrimComp->IsCollisionEnabled()))
				{
					if (bIgnoreStaticMeshBoundsExtension)
					{
						const UStaticMeshComponent* MeshComp = Cast<const UStaticMeshComponent>(PrimComp);
						if (MeshComp && MeshComp->GetStaticMesh())
						{
							UStaticMesh* StaticMesh = MeshComp->GetStaticMesh();

							// nullify bound extension of static meshes.
							FBox MeshBox = MeshComp->Bounds.GetBox();
							MeshBox.Max -= StaticMesh->PositiveBoundsExtension;
							MeshBox.Min += StaticMesh->NegativeBoundsExtension;
							Box += MeshBox;
							continue;
						}
					}

					Box += PrimComp->Bounds.GetBox();
				}
			}
		}

		return Box;
	}

	FBox CalculateActorLocalBounds(const AActor* Actor, bool bIgnoreStaticMeshBoundsExtension)
	{
		FBox Box(ForceInit);

		if (Actor)
		{
			const FTransform& ActorToWorld = Actor->GetTransform();
			const FTransform WorldToActor = ActorToWorld.Inverse();

			const bool bNonColliding = true;
			TInlineComponentArray<UPrimitiveComponent*> PrimitiveComponents(Actor);
			for (const UPrimitiveComponent* PrimComp : PrimitiveComponents)
			{
				// Only use collidable components to find collision bounding box.
				if (PrimComp && (bNonColliding || PrimComp->IsCollisionEnabled()) && !PrimComp->bUseAttachParentBound)
				{
					const FTransform ComponentToActor = PrimComp->GetComponentTransform() * WorldToActor;

					if (bIgnoreStaticMeshBoundsExtension)
					{
						const UStaticMeshComponent* MeshComp = Cast<const UStaticMeshComponent>(PrimComp);
						if (MeshComp && MeshComp->GetStaticMesh())
						{
							UStaticMesh* StaticMesh = MeshComp->GetStaticMesh();

							// nullify bound extension of static meshes.
							FBox MeshBox = MeshComp->CalcBounds(ComponentToActor).GetBox();
							MeshBox.Max -= StaticMesh->PositiveBoundsExtension;
							MeshBox.Min += StaticMesh->NegativeBoundsExtension;
							Box += MeshBox;
							continue;
						}
					}

					Box += PrimComp->CalcBounds(ComponentToActor).GetBox();
				}
			}
		}

		return Box;
	}

	/// copied and modified from Engine/Source/Editor/UnrealEd/Private/Kismet2/DebuggerCommands.cpp FPlayWorldCommandCallbacks::StartPlayFromHere
	void StartPlayFromLocation(const FVector& StartLocation)
	{
		if (!GUnrealEd) return;

		// Is a PIE session already running?  If so we close it first
		if (GUnrealEd->PlayWorld != NULL)
		{
			GUnrealEd->EndPlayMap();
		}

		FRequestPlaySessionParams RequestPlayParams;
		RequestPlayParams.StartLocation = StartLocation;

		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		TSharedPtr<IAssetViewport> ActiveLevelViewport = LevelEditorModule.GetFirstActiveViewport();

		if (ActiveLevelViewport.IsValid())
		{
			RequestPlayParams.DestinationSlateViewport = ActiveLevelViewport;

			if (ActiveLevelViewport->GetAssetViewportClient().IsPerspective())
			{
				RequestPlayParams.StartRotation = ActiveLevelViewport->GetAssetViewportClient().GetViewRotation();
			}
		}

		GUnrealEd->RequestPlaySession(RequestPlayParams);
	}

	/// copied and modified from Engine/Source/Editor/UnrealEd/Private/Factories/ActorFactory.cpp CreateBrushForVolumeActor
	void CreateBrushForVolumeActor(AVolume* Volume, UBrushBuilder* BrushBuilder)
	{
		if (Volume && BrushBuilder)
		{
			// this code builds a brush for the new actor
			Volume->PreEditChange(NULL);

			Volume->PolyFlags = 0;
			Volume->Brush = NewObject<UModel>(Volume, NAME_None, RF_Transactional);
			Volume->Brush->Initialize(nullptr, true);
			Volume->Brush->Polys = NewObject<UPolys>(Volume->Brush, NAME_None, RF_Transactional);
			Volume->GetBrushComponent()->Brush = Volume->Brush;

			Volume->BrushBuilder = DuplicateObject<UBrushBuilder>(BrushBuilder, Volume);

			BrushBuilder->Build(Volume->GetWorld(), Volume);

			FBSPOps::csgPrepMovingBrush(Volume);

			// Set the texture on all polys to NULL.  This stops invisible textures
			// dependencies from being formed on volumes.
			if (Volume->Brush)
			{
				for (int32 poly = 0; poly < Volume->Brush->Polys->Element.Num(); ++poly)
				{
					FPoly* Poly = &(Volume->Brush->Polys->Element[poly]);
					Poly->Material = NULL;
				}
			}

			Volume->PostEditChange();
		}
	}
}

#undef LOCTEXT_NAMESPACE
