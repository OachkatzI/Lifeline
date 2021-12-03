// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LevelStreamingActorEditorBase.h"
#include <LevelStreamBindingMode.h>
#include "LevelStreamingVolumeEditor.generated.h"

UCLASS()
class LEVELSTREAMASSISTANT_API ULevelStreamingVolumeEditor : public ULevelStreamingActorEditorBase
{
	GENERATED_BODY()

public:
	ULevelStreamingVolumeEditor();

	virtual TSubclassOf<AActor> GetTargetType() const override final;

	virtual const FSlateBrush* GetDragAndDropActorIcon() const override;
	
	virtual void ExtendActorsContextMenu(FExtender& MenuExtender, TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const override;

	UPROPERTY(EditAnywhere, Config, Category = "Highlight Settings")
	FLinearColor LoadColor = FLinearColor::Green.CopyWithNewOpacity(0.3f);

private:
	void OnExtendActorsContextMenu(FMenuBuilder& MenuBuilder, TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const;

	void ExecuteAction_AutoBind(TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const;

	class FBindingModeLoad : public FLevelStreamBindingMode
	{
		TWeakObjectPtr<ULevelStreamingVolumeEditor> Editor;

	public:
		FBindingModeLoad(TWeakObjectPtr<ULevelStreamingVolumeEditor> Editor) : Editor(Editor) { }

		virtual void Set(AActor& Target, ULevelStreaming& LevelStreaming) const override;
		virtual void Clear(AActor& Target, ULevelStreaming& LevelStreaming) const override;
		virtual bool IsSet(const AActor& Target, const ULevelStreaming& LevelStreaming) const override;
		virtual FText GetToolTip() const override;
		virtual FLinearColor GetColor() const override;
	};
};
