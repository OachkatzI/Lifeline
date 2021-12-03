// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "LevelStreamingTriggerEditor.h"
#include "GameFramework/Actor.h"
#include "Engine/LevelStreaming.h"
#include "LevelStreamingTrigger.h"
#include "LevelStreamAssistantStyle.h"
#include "GateComponent.h"

#define LOCTEXT_NAMESPACE "ULevelStreamingTriggerEditor"

ULevelStreamingTriggerEditor::ULevelStreamingTriggerEditor()
{
	BindingModes =
	{
		MakeShareable(new FBindingMode(this, true)), // load
		MakeShareable(new FBindingMode(this, false)) // unload
	};
}

void ULevelStreamingTriggerEditor::SetBounds(AActor& Target, const FBox& Bounds) const
{
	if (ALevelStreamingTrigger* Trigger = Cast<ALevelStreamingTrigger>(&Target))
	{
		UGateComponent* TriggerComponent = Trigger->GetTriggerComponent();
		if (ensure(TriggerComponent))
		{
			FVector TriggerExtent = Bounds.GetExtent().GetAbs();
			FVector TriggerCenter = Bounds.GetCenter();

			Target.Modify();
			Target.SetActorTransform(FTransform(FRotator::ZeroRotator, TriggerCenter, FVector::OneVector));

			TriggerComponent->Modify();
			TriggerComponent->SetBoxExtent(TriggerExtent);
		}
	}
}

TSubclassOf<AActor> ULevelStreamingTriggerEditor::GetTargetType() const
{
	return ALevelStreamingTrigger::StaticClass();
}

const FSlateBrush* ULevelStreamingTriggerEditor::GetDragAndDropActorIcon() const
{
	return FLevelStreamAssistantStyle::Get().GetBrush("LSA.Level.Zone.Green");
}

void ULevelStreamingTriggerEditor::FBindingMode::Set(AActor& Target, ULevelStreaming& LevelStreaming) const
{
	ALevelStreamingTrigger& Trigger = Get<ALevelStreamingTrigger>(Target);

	Trigger.Modify();

	if (IsLoadingMode)
	{
		Trigger.LevelsToLoad.AddUnique(LevelStreaming.GetWorldAsset());
	}
	else
	{
		Trigger.LevelsToUnload.AddUnique(LevelStreaming.GetWorldAsset());
	}
}

void ULevelStreamingTriggerEditor::FBindingMode::Clear(AActor& Target, ULevelStreaming& LevelStreaming) const
{
	ALevelStreamingTrigger& Trigger = Get<ALevelStreamingTrigger>(Target);

	Trigger.Modify();

	if (IsLoadingMode)
	{
		Trigger.LevelsToLoad.Remove(LevelStreaming.GetWorldAsset()); // remove all instances of the level name
	}
	else
	{
		Trigger.LevelsToUnload.Remove(LevelStreaming.GetWorldAsset());
	}
}

bool ULevelStreamingTriggerEditor::FBindingMode::IsSet(const AActor& Target, const ULevelStreaming& LevelStreaming) const
{
	const ALevelStreamingTrigger& Trigger = Get<ALevelStreamingTrigger>(Target);

	if (IsLoadingMode)
	{
		return Trigger.LevelsToLoad.Contains(LevelStreaming.GetWorldAsset());
	}
	else
	{
		return Trigger.LevelsToUnload.Contains(LevelStreaming.GetWorldAsset());
	}
}

FText ULevelStreamingTriggerEditor::FBindingMode::GetToolTip() const
{
	return IsLoadingMode
		? LOCTEXT("ToolTipModifyLevelBindingLoad", "level is loaded when player enters this trigger.")
		: LOCTEXT("ToolTipModifyLevelBindingUnload", "level is unloaded when player enters this trigger.");
}

FLinearColor ULevelStreamingTriggerEditor::FBindingMode::GetColor() const
{
	return Editor.IsValid()
		? IsLoadingMode ? Editor->LoadColor : Editor->UnloadColor
		: IsLoadingMode ? FLinearColor::Green : FLinearColor::Red;
}

#undef LOCTEXT_NAMESPACE
