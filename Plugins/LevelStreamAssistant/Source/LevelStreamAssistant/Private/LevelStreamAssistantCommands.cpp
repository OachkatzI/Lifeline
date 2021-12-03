// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "LevelStreamAssistantCommands.h"
#include <LevelStreamAssistantStyle.h>

#define LOCTEXT_NAMESPACE "LevelStreamAssistantCommands"

FLevelStreamAssistantCommands::FLevelStreamAssistantCommands()
	: TCommands<FLevelStreamAssistantCommands>(
		TEXT("LSA.Commands"),
		LOCTEXT("ContextDescription", "Level Stream Assistant Commands"),
		NAME_None,
		FLevelStreamAssistantStyle::GetStyleSetName())
{
}

void FLevelStreamAssistantCommands::RegisterCommands()
{
	UI_COMMAND(Option_ShowOnlyRelatedLevels,
		"Show Only Related Levels",
		"Show only related levels in viewport.",
		EUserInterfaceActionType::ToggleButton,
		FInputChord());

	UI_COMMAND(Option_HighlightRelatedLevels,
		"Highlight Related Levels",
		"Highlight related levels in viewport. (plugin mode must be active)",
		EUserInterfaceActionType::ToggleButton,
		FInputChord());
	
	UI_COMMAND(Option_FocusBoundLevels,
		"Focus Bound Levels",
		"Focus on levels bound to selected actor.",
		EUserInterfaceActionType::ToggleButton,
		FInputChord());

	UI_COMMAND(Option_FocusSelectedActor,
		"Focus Selected Actor",
		"Focus on selected actor in viewport.",
		EUserInterfaceActionType::ToggleButton,
		FInputChord());

	UI_COMMAND(Option_SyncWithViewportSelection,
		"Sync With Viewport Selection",
		"Sync actor list selection with viewport selection.",
		EUserInterfaceActionType::ToggleButton,
		FInputChord());

	UI_COMMAND(Option_PreviewLevelVisibility,
		"Preview Level Visibility",
		"Preview visibility of hovered levels for binding/unbinding.",
		EUserInterfaceActionType::ToggleButton,
		FInputChord());

	UI_COMMAND(Option_HighlightSelections,
		"Highlight Selections",
		"Highlight selected actor and hovered levels. (plugin mode must be active)",
		EUserInterfaceActionType::ToggleButton,
		FInputChord());

	UI_COMMAND(Option_PreviewSizeMap,
		"Preview Size Map",
		"Calculate disk/memory size of levels bound to the selected actor. (disable if it causes hitches.)",
		EUserInterfaceActionType::ToggleButton,
		FInputChord());

	UI_COMMAND(OpenEditorDetailsPanel,
		"Editor Details Panel",
		"Open details panel for the current level streaming editor.",
		EUserInterfaceActionType::Button,
		FInputChord());

	UI_COMMAND(MakeCurrent, 
		"Make Current", 
		"Make This Level The Current Level.", 
		EUserInterfaceActionType::Button, 
		FInputChord(EKeys::Enter));

	UI_COMMAND(MoveSelectedActorsToLevel,
		"Move Selected Actors To Level",
		"Move Selected Actors To This Level.",
		EUserInterfaceActionType::Button,
		FInputChord());

	UI_COMMAND(ChangeStreamingMethodToBlueprint,
		"Blueprint",
		"Changes the streaming method for the selected levels to Blueprint streaming",
		EUserInterfaceActionType::Check,
		FInputChord());
	
	UI_COMMAND(ChangeStreamingMethodToAlwaysLoaded,
		"Always Loaded", 
		"Changes the streaming method for the selected levels to be always loaded", 
		EUserInterfaceActionType::Check, 
		FInputChord());

	UI_COMMAND(FindLevelsInContentBrowser,
		"Find in Content Browser",
		"Find the selected levels in the Content Browser",
		EUserInterfaceActionType::Button,
		FInputChord());

	UI_COMMAND(SetFoldersNewColor,
		"Change Color",
		"Change the color of the selected folders using Color Picker.",
		EUserInterfaceActionType::Button,
		FInputChord());

	UI_COMMAND(CreateFolder,
		"Create Folder",
		"Create a new folder for selected Items.",
		EUserInterfaceActionType::Button,
		FInputChord());

	UI_COMMAND(CreateSubFolder,
		"Create Sub Folder",
		"Create a new folder under the selected folder.",
		EUserInterfaceActionType::Button,
		FInputChord());
}

const FLevelStreamAssistantCommands& FLevelStreamAssistantCommands::GetSafe()
{
	if (!ensure(FLevelStreamAssistantCommands::IsRegistered()))
	{
		FLevelStreamAssistantCommands::Register();
	}
	return FLevelStreamAssistantCommands::Get();
}

#undef LOCTEXT_NAMESPACE
