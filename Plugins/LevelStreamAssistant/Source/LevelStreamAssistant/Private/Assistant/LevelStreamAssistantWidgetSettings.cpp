// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Assistant/LevelStreamAssistantWidgetSettings.h"
#include "LevelStreamingVolumeEditor.h"
#include "Engine/LevelStreamingVolume.h"

ULevelStreamAssistantWidgetSettings::ULevelStreamAssistantWidgetSettings()
{
	EditorType = ULevelStreamingVolumeEditor::StaticClass();
	TargetType = ALevelStreamingVolume::StaticClass();
	bShowOnlyRelatedLevels = false;
	bHighlightRelatedLevels = true;
	bFocusOnBoundLevels = false;
	bFocusOnSelectedActor = false;
	bSyncListSelectionWithViewportSelection = true;
	bPreviewLevelVisibility = false;
	bHighlightSelections = true;
	bCalculateSizeMap = false;
}
