// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "LevelStreamingVolumeEditor.h"
#include "LevelStreamAssistantStyle.h"
#include <Utilities/LevelStreamAssistantUtils.h>
#include "Engine/LevelStreamingVolume.h"
#include "Engine/LevelStreaming.h"
#include <Framework/MultiBox/MultiBoxBuilder.h>
#include <ScopedTransaction.h>
#include <Assistant/LevelStreamAssistantHierarchyView.h>

#define LOCTEXT_NAMESPACE "ULevelStreamingVolumeEditor"

ULevelStreamingVolumeEditor::ULevelStreamingVolumeEditor()
{
	BindingModes =
	{
		MakeShareable(new FBindingModeLoad(this))
	};
}

TSubclassOf<AActor> ULevelStreamingVolumeEditor::GetTargetType() const
{
	return ALevelStreamingVolume::StaticClass();
}

const FSlateBrush* ULevelStreamingVolumeEditor::GetDragAndDropActorIcon() const
{
	return FLevelStreamAssistantStyle::Get().GetBrush("LSA.Level.Zone.Orange");
}

void ULevelStreamingVolumeEditor::ExtendActorsContextMenu(FExtender& MenuExtender, TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const
{
	Super::ExtendActorsContextMenu(MenuExtender, HierarchyView);

	MenuExtender.AddMenuExtension("Actor", EExtensionHook::After, nullptr,
		FMenuExtensionDelegate::CreateUObject(this, &ULevelStreamingVolumeEditor::OnExtendActorsContextMenu, HierarchyView)
	);
}

void ULevelStreamingVolumeEditor::OnExtendActorsContextMenu(FMenuBuilder& MenuBuilder, TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const
{
	MenuBuilder.AddMenuEntry(
		LOCTEXT("LabelAutoBind", "Auto Bind"),
		LOCTEXT("ToolTipAutoBind", "Automatically bind levels to selected actor."),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateUObject(this, &ULevelStreamingVolumeEditor::ExecuteAction_AutoBind, HierarchyView)
		)
	);
}

void ULevelStreamingVolumeEditor::ExecuteAction_AutoBind(TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const
{
	TArray<AActor*> SelectedActors = HierarchyView->GetSelectedLevelStreamingActors();

	TArray<ULevelStreaming*> StreamingLevels = LevelStreamAssistantUtils::GetStreamingLevelsFromEditorWorld();

	if (SelectedActors.Num() > 0 && StreamingLevels.Num() > 0)
	{
		FScopedTransaction Transaction(LOCTEXT("TransactionAutoBindLevels", "Auto Bind To Levels"));

		// clear bindings first
		for (ULevelStreaming* Level : StreamingLevels)
		{
			for (AActor* SelectedActor : SelectedActors)
			{
				SetBinding(SelectedActor, Level, /*NewBindingMode*/nullptr);
			}
		}

		for (AActor* SelectedActor : SelectedActors)
		{
			FBox ActorBounds = GetBounds(SelectedActor);

			TSharedPtr<FLevelStreamBindingMode> FirstBindingMode = BindingModes.Num() > 0 ? BindingModes[0] : nullptr;

			// bind overlapping levels to selected level streaming actor
			for (ULevelStreaming* Level : StreamingLevels)
			{
				if (Level)
				{
					FBox LevelBounds = LevelStreamAssistantUtils::CalculateLevelBounds(Level->GetLoadedLevel(), true);

					if (ActorBounds.Intersect(LevelBounds))
					{
						SetBinding(SelectedActor, Level, FirstBindingMode);
					}
				}
			}
		}
	}
}

void ULevelStreamingVolumeEditor::FBindingModeLoad::Set(AActor& Target, ULevelStreaming& LevelStreaming) const
{
	ALevelStreamingVolume& Volume = Get<ALevelStreamingVolume>(Target);

	LevelStreaming.Modify();
	LevelStreaming.EditorStreamingVolumes.AddUnique(&Volume);
}

void ULevelStreamingVolumeEditor::FBindingModeLoad::Clear(AActor& Target, ULevelStreaming& LevelStreaming) const
{
	ALevelStreamingVolume& Volume = Get<ALevelStreamingVolume>(Target);

	LevelStreaming.Modify();
	LevelStreaming.EditorStreamingVolumes.Remove(&Volume); // remove all instances of the zone
}

bool ULevelStreamingVolumeEditor::FBindingModeLoad::IsSet(const AActor& Target, const ULevelStreaming& LevelStreaming) const
{
	const ALevelStreamingVolume& Volume = Get<ALevelStreamingVolume>(Target);
	return LevelStreaming.EditorStreamingVolumes.Contains(&Volume);
}

FText ULevelStreamingVolumeEditor::FBindingModeLoad::GetToolTip() const
{
	return LOCTEXT("ToolTip_BindingMode_Load", "streaming of this level is controlled by selected actors.");
}

FLinearColor ULevelStreamingVolumeEditor::FBindingModeLoad::GetColor() const
{
	return Editor.IsValid() ? Editor->LoadColor : FLinearColor::Green;
}

#undef LOCTEXT_NAMESPACE
