// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LevelStreamAssistantWidgetSettings.generated.h"

class ULevelStreamingActorEditorBase;

/**
 * settings for SLevelStreamAssistantWidget.
 */
UCLASS(Config=EditorPerProjectUserSettings)
class ULevelStreamAssistantWidgetSettings : public UObject
{
	GENERATED_BODY()
	
public:
	ULevelStreamAssistantWidgetSettings();
	
	virtual bool IsEditorOnly() const override final { return true; }

	UPROPERTY(Config)
	TSoftClassPtr<ULevelStreamingActorEditorBase> EditorType;

	UPROPERTY()
	TSubclassOf<AActor> TargetType;

	// Only levels bound to selected actor will be visible.
	UPROPERTY(Config)
	uint8 bShowOnlyRelatedLevels : 1;

	// levels bound to selected actor will be highlighted.
	UPROPERTY(Config)
	uint8 bHighlightRelatedLevels : 1;

	// When actor selection changes, focus on levels bound to selected actor.
	UPROPERTY(Config)
	uint8 bFocusOnBoundLevels : 1;

	// When actor selection changes, focus on selected actor in viewport.
	UPROPERTY(Config)
	uint8 bFocusOnSelectedActor : 1;

	// Synchronize actor list selection with viewport selection.
	UPROPERTY(Config)
	uint8 bSyncListSelectionWithViewportSelection : 1;

	// When hovering over levels on list view, preview binding/unbinding by making levels visible/hidden.
	UPROPERTY(Config)
	uint8 bPreviewLevelVisibility : 1;

	// Render highlights in viewport.
	UPROPERTY(Config)
	uint8 bHighlightSelections : 1;

	// Show disk/memory size overview on size map button.
	UPROPERTY(Config)
	uint8 bCalculateSizeMap : 1;
};
