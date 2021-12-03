// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FLevelStreamAssistantCommands : public TCommands<FLevelStreamAssistantCommands>
{
public:
	FLevelStreamAssistantCommands();

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> Option_ShowOnlyRelatedLevels;
	TSharedPtr<FUICommandInfo> Option_HighlightRelatedLevels;
	TSharedPtr<FUICommandInfo> Option_FocusBoundLevels;
	TSharedPtr<FUICommandInfo> Option_FocusSelectedActor;
	TSharedPtr<FUICommandInfo> Option_SyncWithViewportSelection;
	TSharedPtr<FUICommandInfo> Option_PreviewLevelVisibility;
	TSharedPtr<FUICommandInfo> Option_HighlightSelections;
	TSharedPtr<FUICommandInfo> Option_PreviewSizeMap;
	TSharedPtr<FUICommandInfo> OpenEditorDetailsPanel;

	TSharedPtr<FUICommandInfo> MakeCurrent;
	TSharedPtr<FUICommandInfo> MoveSelectedActorsToLevel;
	TSharedPtr<FUICommandInfo> ChangeStreamingMethodToBlueprint;
	TSharedPtr<FUICommandInfo> ChangeStreamingMethodToAlwaysLoaded;
	TSharedPtr<FUICommandInfo> FindLevelsInContentBrowser;

	TSharedPtr<FUICommandInfo> CreateFolder;
	TSharedPtr<FUICommandInfo> CreateSubFolder;
	TSharedPtr<FUICommandInfo> SetFoldersNewColor;

	static const FLevelStreamAssistantCommands& GetSafe();
};
