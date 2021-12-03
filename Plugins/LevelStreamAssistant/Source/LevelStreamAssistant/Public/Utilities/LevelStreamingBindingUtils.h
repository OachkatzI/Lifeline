// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class ULevelStreaming;
class AActor;
class FLevelStreamBindingMode;

namespace LevelStreamAssistant
{
	class ILevelStreamingEditor;
	struct FBaseItem;

	enum class EBindingModeResult
	{
		Success,
		MultipleBindings,
		NoBinding
	};

	EBindingModeResult GetItemBindingModeToSelectedActors(TSharedPtr<ILevelStreamingEditor> LevelStreamingEditor, TSharedPtr<FBaseItem> Item, TSharedPtr<FLevelStreamBindingMode>& OutBindingMode);

	EBindingModeResult GetBindingModeToSelectedActors(TSharedPtr<ILevelStreamingEditor> LevelStreamingEditor, ULevelStreaming* Level, TSharedPtr<FLevelStreamBindingMode>& OutBindingMode);

	void SetItemBindingModeToSelectedActors(TSharedPtr<ILevelStreamingEditor> LevelStreamingEditor, TSharedPtr<FBaseItem> Item, TSharedPtr<FLevelStreamBindingMode> NewBindingMode);

	void SetBindingModeToSelectedActors(TSharedPtr<ILevelStreamingEditor> LevelStreamingEditor, ULevelStreaming* Level, TSharedPtr<FLevelStreamBindingMode> NewBindingMode);

	TSharedPtr<FLevelStreamBindingMode> GetFirstBindingMode(TSharedPtr<ILevelStreamingEditor> LevelStreamingEditor);

	bool IsLevelBoundToAnySelectedActor(TSharedPtr<ILevelStreamingEditor> LevelStreamingEditor, ULevelStreaming* InLevel);
};
