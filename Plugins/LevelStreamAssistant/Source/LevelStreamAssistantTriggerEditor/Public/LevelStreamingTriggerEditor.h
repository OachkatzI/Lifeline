// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LevelStreamingActorEditorBase.h"
#include <LevelStreamBindingMode.h>
#include "LevelStreamingTriggerEditor.generated.h"

UCLASS()
class ULevelStreamingTriggerEditor : public ULevelStreamingActorEditorBase
{
	GENERATED_BODY()

public:
	ULevelStreamingTriggerEditor();

	virtual void SetBounds(AActor& Target, const FBox& Bounds) const override;

	virtual TSubclassOf<AActor> GetTargetType() const override final;

	virtual const FSlateBrush* GetDragAndDropActorIcon() const override;

	UPROPERTY(EditAnywhere, Config, Category = "Highlight Settings")
	FLinearColor LoadColor = FLinearColor::Green.CopyWithNewOpacity(0.3);

	UPROPERTY(EditAnywhere, Config, Category = "Highlight Settings")
	FLinearColor UnloadColor = FLinearColor::Red.CopyWithNewOpacity(0.3);

private:

	class FBindingMode : public FLevelStreamBindingMode
	{
		TWeakObjectPtr<ULevelStreamingTriggerEditor> Editor;

		const bool IsLoadingMode; // binding mode is set to load or unload.

	public:
		FBindingMode(TWeakObjectPtr<ULevelStreamingTriggerEditor> Editor, bool bLoad) : Editor(Editor), IsLoadingMode(bLoad) { }

		virtual void Set(AActor& Target, ULevelStreaming& LevelStreaming) const override;
		virtual void Clear(AActor& Target, ULevelStreaming& LevelStreaming) const override;
		virtual bool IsSet(const AActor& Target, const ULevelStreaming& LevelStreaming) const override;
		virtual FText GetToolTip() const override;
		virtual FLinearColor GetColor() const override;
	};
};
