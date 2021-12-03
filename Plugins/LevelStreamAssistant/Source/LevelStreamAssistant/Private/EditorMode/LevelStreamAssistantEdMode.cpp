// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "EditorMode/LevelStreamAssistantEdMode.h"
#include "EditorMode/LevelStreamAssistantEdModeToolkit.h"
#include "EditorMode/LevelStreamAssistantSettings.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"
#include <SceneHighlights.h>

const FEditorModeID FLevelStreamAssistantEdMode::EM_LevelStreamAssistantEdModeId = TEXT("EM_LevelStreamAssistantEdMode");

void FLevelStreamAssistantEdMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FLevelStreamAssistantEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
}

void FLevelStreamAssistantEdMode::Exit()
{
	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// save user settings before exit.
	GetSettings().SaveConfig();

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

void FLevelStreamAssistantEdMode::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	FEdMode::Render(View, Viewport, PDI);

	if(View && PDI) GetSceneHighlights().Render(*View, *PDI);
}

bool FLevelStreamAssistantEdMode::UsesToolkits() const
{
	return true;
}

ULevelStreamAssistantSettings& FLevelStreamAssistantEdMode::GetSettings() const
{
	ULevelStreamAssistantSettings* Settings = GetMutableDefault<ULevelStreamAssistantSettings>();
	check(Settings);
	return *Settings;
}

LevelStreamAssistant::FSceneHighlights& FLevelStreamAssistantEdMode::GetSceneHighlights()
{
	if (!SceneHighlights.IsValid())
	{
		SceneHighlights = MakeShareable(new LevelStreamAssistant::FSceneHighlights);
	}
	return *SceneHighlights;
}
