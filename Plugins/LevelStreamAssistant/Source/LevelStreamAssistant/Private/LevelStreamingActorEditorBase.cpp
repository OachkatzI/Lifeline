// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "LevelStreamingActorEditorBase.h"
#include <LevelStreamBindingMode.h>
#include "Utilities/LevelStreamAssistantUtils.h"
#include "LevelStreamAssistantStyle.h"
#include "Builders/CubeBuilder.h"
#include <Framework/MultiBox/MultiBoxBuilder.h>
#include <Engine/LevelStreaming.h>
#include <ScopedTransaction.h>
#include <Engine/Selection.h>
#include <EngineUtils.h>
#include <Editor.h>
#include <Assistant/LevelStreamAssistantHierarchyView.h>

#define LOCTEXT_NAMESPACE "ULevelStreamingActorEditorBase"

void ULevelStreamingActorEditorBase::SetBounds(AActor& Target, const FBox& Bounds) const
{
	if (!IsActorSupported(&Target)) return;

	if (AVolume* Volume = Cast<AVolume>(&Target))
	{
		FVector TriggerSize = Bounds.GetSize().GetAbs();
		FVector TriggerCenter = Bounds.GetCenter();

		Target.Modify();
		Target.SetActorTransform(FTransform(FRotator::ZeroRotator, TriggerCenter, FVector::OneVector));

		// Make cube shaped volumes
		UCubeBuilder* CubeBuilder = NewObject<UCubeBuilder>();
		CubeBuilder->X = TriggerSize.X;
		CubeBuilder->Y = TriggerSize.Y;
		CubeBuilder->Z = TriggerSize.Z;
		LevelStreamAssistantUtils::CreateBrushForVolumeActor(Volume, CubeBuilder);
	}
}

FBox ULevelStreamingActorEditorBase::GetBounds(const AActor& Target) const
{
	return IsActorSupported(&Target)
		? LevelStreamAssistantUtils::CalculateActorBounds(&Target, true)
		: FBox(ForceInit);
}

void ULevelStreamingActorEditorBase::ExtendActorsContextMenu(FExtender& MenuExtender, TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const
{
	MenuExtender.AddMenuExtension("Actor", EExtensionHook::After, nullptr, 
		FMenuExtensionDelegate::CreateUObject(this, &ULevelStreamingActorEditorBase::OnExtendActorsContextMenu, HierarchyView)
	);
}

TSubclassOf<AActor> ULevelStreamingActorEditorBase::GetTargetType() const 
{
	return AActor::StaticClass();
}

const FSlateBrush* ULevelStreamingActorEditorBase::GetDragAndDropActorIcon() const
{
	return FLevelStreamAssistantStyle::Get().GetBrush("LSA.Level.Zone.Orange");
}

void ULevelStreamingActorEditorBase::SetBounds(AActor* Target, const FBox& Bounds) const
{
	if (IsActorSupported(Target)) SetBounds(*Target, Bounds);
}

FBox ULevelStreamingActorEditorBase::GetBounds(const AActor* Target) const
{
	return IsActorSupported(Target) ? GetBounds(*Target) : FBox(ForceInit);
}

bool ULevelStreamingActorEditorBase::IsActorSupported(const AActor* Actor) const
{
	return Actor && Actor->IsA(GetTargetType());
}

TSharedPtr<FLevelStreamBindingMode> ULevelStreamingActorEditorBase::GetBinding(const AActor* InActor, const ULevelStreaming* InLevel) const
{
	if (IsActorSupported(InActor) && InLevel)
	{
		for (TSharedPtr<FLevelStreamBindingMode> BindingMode : GetBindingModes())
		{
			if (BindingMode.IsValid() && BindingMode->IsSet(*InActor, *InLevel))
			{
				return BindingMode;
			}
		}
	}
	return nullptr;
}

void ULevelStreamingActorEditorBase::SetBinding(AActor* InActor, ULevelStreaming* InLevel, const TSharedPtr<FLevelStreamBindingMode>& NewBindingMode) const
{
	if (IsActorSupported(InActor) && InLevel)
	{
		TSharedPtr<FLevelStreamBindingMode> CurrentBindingMode = GetBinding(InActor, InLevel);

		// switch to new binding mode if it's different.
		if (CurrentBindingMode != NewBindingMode)
		{
			if (CurrentBindingMode.IsValid()) CurrentBindingMode->Clear(*InActor, *InLevel);
			if (NewBindingMode.IsValid()) NewBindingMode->Set(*InActor, *InLevel);
		}
	}
}

void ULevelStreamingActorEditorBase::OnExtendActorsContextMenu(FMenuBuilder& MenuBuilder, TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const
{
	TArray<AActor*> SelectedActors = HierarchyView->GetSelectedLevelStreamingActors();

	if (SelectedActors.Num() > 0)
	{
		if (SelectedActors.Num() == 1)
		{
			MenuBuilder.AddMenuEntry(
				LOCTEXT("LabelPlay", "Play from here"),
				LOCTEXT("ToolTipPlay", "Play from here"),
				FSlateIcon(FLevelStreamAssistantStyle::GetStyleSetName(), "LSA.Tools.Play"),
				FUIAction(
					FExecuteAction::CreateUObject(this, &ULevelStreamingActorEditorBase::ExecuteAction_PlayFromHere, SelectedActors[0])
				)
			);
		}

		MenuBuilder.AddMenuEntry(
			LOCTEXT("LabelBoundFromLevels", "Calculate Bounds From Levels"),
			LOCTEXT("ToolTipBoundFromLevels", "Calculate and set the bounding box of selected level streaming actor from selected levels."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateUObject(this, &ULevelStreamingActorEditorBase::ExecuteAction_CalculateBoundFromLevels, HierarchyView)
			)
		);
		MenuBuilder.AddMenuEntry(
			LOCTEXT("LabelBoundFromActors", "Calculate Bounds From Actors"),
			LOCTEXT("ToolTipBoundFromActors", "Calculate and set the bounding box of selected level streaming actor from other selected actors in viewport."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateUObject(this, &ULevelStreamingActorEditorBase::ExecuteAction_CalculateBoundFromActors, HierarchyView)
			)
		);
		MenuBuilder.AddMenuEntry(
			LOCTEXT("LabelSelectActorsInside", "Select Actors Inside"),
			LOCTEXT("ToolTipSelectActorsInside", "Select actors inside bounds of the selected level streaming actor."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateUObject(this, &ULevelStreamingActorEditorBase::ExecuteAction_SelectActorsInside, HierarchyView)
			)
		);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("LabelBindAll", "Bind All"),
			LOCTEXT("ToolTipBindAll", "Bind all levels to selected actor."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateUObject(this, &ULevelStreamingActorEditorBase::ExecuteAction_BindAll, HierarchyView)
			)
		);
		MenuBuilder.AddMenuEntry(
			LOCTEXT("LabelUnbindAll", "Unbind All"),
			LOCTEXT("ToolTipUnbindAll", "Unbind all leves from selected actor."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateUObject(this, &ULevelStreamingActorEditorBase::ExecuteAction_UnbindAll, HierarchyView)
			)
		);
	}
}

void ULevelStreamingActorEditorBase::ExecuteAction_PlayFromHere(AActor* SelectedActor) const
{
	if (SelectedActor) LevelStreamAssistantUtils::StartPlayFromLocation(SelectedActor->GetActorLocation());
}

void ULevelStreamingActorEditorBase::ExecuteAction_BindAll(TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const
{
	TArray<AActor*> SelectedActors = HierarchyView->GetSelectedLevelStreamingActors();

	TArray<ULevelStreaming*> StreamingLevels = LevelStreamAssistantUtils::GetStreamingLevelsFromEditorWorld();

	if (SelectedActors.Num() > 0 && StreamingLevels.Num() > 0)
	{
		FScopedTransaction Transaction(LOCTEXT("TransactionBindAllLevels", "Bind To All Levels"));

		TSharedPtr<FLevelStreamBindingMode> FirstBindingMode = BindingModes.Num() > 0 ? BindingModes[0] : nullptr;

		for (AActor* SelectedActor : SelectedActors)
		{
			for (ULevelStreaming* Level : StreamingLevels)
			{
				TSharedPtr<FLevelStreamBindingMode> BindingMode = GetBinding(SelectedActor, Level);

				// if binding mode is already set, don't change it. otherwise set to first binding mode.
				if (!BindingMode.IsValid())
				{
					SetBinding(SelectedActor, Level, FirstBindingMode);
				}
			}
		}
	}
}

void ULevelStreamingActorEditorBase::ExecuteAction_UnbindAll(TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const
{
	TArray<AActor*> SelectedActors = HierarchyView->GetSelectedLevelStreamingActors();

	TArray<ULevelStreaming*> StreamingLevels = LevelStreamAssistantUtils::GetStreamingLevelsFromEditorWorld();
	
	if (SelectedActors.Num() > 0 && StreamingLevels.Num())
	{
		FScopedTransaction Transaction(LOCTEXT("TransactionUnbindAllLevels", "Unbind From All Levels"));

		for (ULevelStreaming* Level : StreamingLevels)
		{
			for (AActor* SelectedActor : SelectedActors)
			{
				SetBinding(SelectedActor, Level, /*NewBindingMode*/nullptr);
			}
		}
	}
}

void ULevelStreamingActorEditorBase::ExecuteAction_CalculateBoundFromLevels(TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const
{
	TArray<AActor*> SelectedActors = HierarchyView->GetSelectedLevelStreamingActors();

	UWorld* World = LevelStreamAssistantUtils::GetWorldFromEditor();
	
	TArray<ULevelStreaming*> StreamingLevels;
	if (World) StreamingLevels = World->GetStreamingLevels();
	
	if (SelectedActors.Num() > 0 && StreamingLevels.Num() > 0)
	{
		FScopedTransaction Transaction(LOCTEXT("TransactionBoundFromLevels", "Calculate Bound From Levels"));

		for (AActor* SelectedActor : SelectedActors)
		{
			FBox Bounds = FBox(ForceInit);
			for (ULevelStreaming* LevelStreaming : StreamingLevels)
			{
				if (GetBinding(SelectedActor, LevelStreaming).IsValid())
				{
					Bounds += LevelStreamAssistantUtils::CalculateLevelBounds(LevelStreaming->GetLoadedLevel(), true);
				}
			}

			if (Bounds.IsValid)
			{
				SetBounds(SelectedActor, Bounds);
			}
		}
	}
}

void ULevelStreamingActorEditorBase::ExecuteAction_CalculateBoundFromActors(TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const
{
	TArray<AActor*> SelectedActors = HierarchyView->GetSelectedLevelStreamingActors();

	if (GEditor && SelectedActors.Num() > 0)
	{
		FBox Bounds = FBox(ForceInit);
		if (USelection* ActorSelection = GEditor->GetSelectedActors())
		{
			TArray<AActor*> OtherSelectedActors;
			ActorSelection->GetSelectedObjects<AActor>(OtherSelectedActors);
			Bounds = LevelStreamAssistantUtils::CalculateBoundsFromActors(OtherSelectedActors, true);
		}

		if (Bounds.IsValid)
		{
			FScopedTransaction Transaction(LOCTEXT("TransactionBoundFromActors", "Calculate Bound From Actors"));
			{
				for (AActor* SelectedActor : SelectedActors)
				{
					if (SelectedActor)
					{
						SetBounds(*SelectedActor, Bounds);
					}
				}
			}
		}
	}
}

void ULevelStreamingActorEditorBase::ExecuteAction_SelectActorsInside(TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const
{
	TArray<AActor*> SelectedActors = HierarchyView->GetSelectedLevelStreamingActors();

	if (GEditor && SelectedActors.Num() > 0)
	{
		UWorld* World = GEditor->GetEditorWorldContext().World();
		if (!World) return;

		FScopedTransaction Transaction(LOCTEXT("TransactionSelectActorsInside", "Select Actors Inside"));

		TMap<AActor*, FBox> LevelStreamingActorsBoxMap;
		TMap<AActor*, FBox> OtherActorsBoxMap;

		for (AActor* SelectedActor : SelectedActors)
		{
			if (SelectedActor)
			{
				FBox ActorBounds = GetBounds(*SelectedActor);
				LevelStreamingActorsBoxMap.Add(SelectedActor, ActorBounds);
			}
		}

		for (AActor* Actor : TActorRange<AActor>(World))
		{
			// include all actors except level streaming actors.
			if (Actor && !Actor->GetClass()->IsChildOf(GetTargetType()))
			{
				FBox ActorBounds = LevelStreamAssistantUtils::CalculateActorBounds(Actor, true);
				OtherActorsBoxMap.Add(Actor, ActorBounds);
			}
		}

		GEditor->SelectNone(/*bNoteSelectionChange*/ false, /*bDeselectBSPSurfs*/ true);

		for (TPair<AActor*, FBox> LevelStreamingActorBox : LevelStreamingActorsBoxMap)
		{
			for (TPair<AActor*, FBox> OtherActorBox : OtherActorsBoxMap)
			{
				// select actors that are inside the selected level streaming actors within 50cm margin. todo: allow modifying the margin
				if (LevelStreamingActorBox.Value.ExpandBy(50.f).IsInside(OtherActorBox.Value))
				{
					GEditor->SelectActor(OtherActorBox.Key, /*bInSelected*/true, /*bNotify*/false, /*bSelectEvenIfHidden*/true);
				}
			}
		}

		GEditor->NoteSelectionChange();
	}
}

#undef LOCTEXT_NAMESPACE
