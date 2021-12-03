// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class AActor;
class ULevelStreaming;
namespace LevelStreamAssistant { class ILevelStreamingEditor; }

// minimal interface for getting information about level hierarchy and actor hierarchy from level stream assistant widget.
struct FLevelStreamAssistantHierarchyView
{
public:
	FLevelStreamAssistantHierarchyView(TSharedPtr<LevelStreamAssistant::ILevelStreamingEditor> InLevelStreamingEditor)
		: LevelStreamingEditor(InLevelStreamingEditor) { }

	LEVELSTREAMASSISTANT_API TArray<AActor*> GetLevelStreamingActors() const;
	LEVELSTREAMASSISTANT_API TArray<ULevelStreaming*> GetStreamingLevels() const;

	LEVELSTREAMASSISTANT_API TArray<AActor*> GetSelectedLevelStreamingActors() const;
	LEVELSTREAMASSISTANT_API TArray<ULevelStreaming*> GetSelectedStreamingLevels() const;

	LEVELSTREAMASSISTANT_API void ClearSelectedLevelStreamingActors();
	LEVELSTREAMASSISTANT_API void ClearSelectedStreamingLevels();

	LEVELSTREAMASSISTANT_API void SelectLevelStreamingActor(AActor* InActor);
	LEVELSTREAMASSISTANT_API void SelectStreamingLevel(ULevelStreaming* InStreamingLevel);

	LEVELSTREAMASSISTANT_API void NotifySelectedActorsChanged() const;
	LEVELSTREAMASSISTANT_API void NotifySelectedLevelsChanged() const;
	LEVELSTREAMASSISTANT_API void NotifyLevelBindingChanged() const;

private:
	TSharedPtr<LevelStreamAssistant::ILevelStreamingEditor> LevelStreamingEditor;
};
