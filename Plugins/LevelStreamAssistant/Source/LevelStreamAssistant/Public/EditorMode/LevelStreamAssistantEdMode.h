// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdMode.h"

class ULevelStreamAssistantSettings;
namespace LevelStreamAssistant { class FSceneHighlights; }

/**
* Level stream assistant editor mode
*/
class FLevelStreamAssistantEdMode : public FEdMode
{
public:
	const static FEditorModeID EM_LevelStreamAssistantEdModeId;

public:
	FLevelStreamAssistantEdMode() {}
	virtual ~FLevelStreamAssistantEdMode() {}

	// FEdMode interface
	virtual void Enter() override;
	virtual void Exit() override;
	//virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;
	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;
	//virtual void ActorSelectionChangeNotify() override;
	virtual bool UsesToolkits() const override;
	//virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	// End of FEdMode interface

	ULevelStreamAssistantSettings& GetSettings() const;

	LevelStreamAssistant::FSceneHighlights& GetSceneHighlights();

private:
	TSharedPtr<LevelStreamAssistant::FSceneHighlights> SceneHighlights;
};
