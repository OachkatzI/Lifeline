// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LevelStreamingActorEditorBase.h"
#include "BlueprintLevelStreamingActorEditorBase.generated.h"

struct FLevelStreamAssistantHierarchyView;

USTRUCT(BlueprintType, meta = (DisplayName = "Level Stream Binding Mode"))
struct FLevelStreamBindingMode_Blueprint_Handle
{
	GENERATED_BODY()

public:
	FLevelStreamBindingMode_Blueprint_Handle() { }

	FLevelStreamBindingMode_Blueprint_Handle(TSharedPtr<FLevelStreamBindingMode> InBindingMode)
		: BindingMode(InBindingMode) { }

	TSharedPtr<FLevelStreamBindingMode> Get() const
	{
		return BindingMode;
	}

private:
	TSharedPtr<FLevelStreamBindingMode> BindingMode;
};

USTRUCT(BlueprintType, BlueprintInternalUseOnly, meta = (DisplayName = "Level Stream Assistant Hierarchy View"))
struct FLevelStreamAssistantHierarchyView_Blueprint
{
	GENERATED_BODY()

public:
	FLevelStreamAssistantHierarchyView_Blueprint() { }

	FLevelStreamAssistantHierarchyView_Blueprint(TSharedPtr<FLevelStreamAssistantHierarchyView> InHierarchyView)
		: HierarchyView(InHierarchyView) { }

	TArray<AActor*> GetLevelStreamingActors() const;
	TArray<ULevelStreaming*> GetStreamingLevels() const;

	TArray<AActor*> GetSelectedLevelStreamingActors() const;
	TArray<ULevelStreaming*> GetSelectedStreamingLevels() const;

	void ClearSelectedLevelStreamingActors();
	void ClearSelectedStreamingLevels();

	void SelectLevelStreamingActor(AActor* InActor);
	void SelectStreamingLevel(ULevelStreaming* InStreamingLevel);

	void NotifySelectedActorsChanged() const;
	void NotifySelectedLevelsChanged() const;
	void NotifyLevelBindingChanged() const;

private:
	TSharedPtr<FLevelStreamAssistantHierarchyView> HierarchyView;
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FLevelStreamAssistant_HierarchyView_MenuItem_ExecuteAction, const FLevelStreamAssistantHierarchyView_Blueprint&, HierarchyView);

USTRUCT(BlueprintType, BlueprintInternalUseOnly, meta = (DisplayName = "Level Stream Assistant Hierarchy Item Context Menu Builder"))
struct FLevelStreamAssistant_HierarchyView_ContextMenuBuilder
{
	GENERATED_BODY()

public:
	FLevelStreamAssistant_HierarchyView_ContextMenuBuilder() { }

	FLevelStreamAssistant_HierarchyView_ContextMenuBuilder(FMenuBuilder& InMenuBuilder, const FLevelStreamAssistantHierarchyView_Blueprint& InHierarchyView)
		: MenuBuilder(&InMenuBuilder), HierarchyView(InHierarchyView) { }

	void AddMenuItem(FText Label, FText ToolTip, FLevelStreamAssistant_HierarchyView_MenuItem_ExecuteAction ExecuteAction);

private:
	FMenuBuilder* MenuBuilder = nullptr;
	FLevelStreamAssistantHierarchyView_Blueprint HierarchyView;
};

/**
 * This class can be inherited to make level streaming editors in blueprint.
 */
UCLASS(Blueprintable, Abstract, Experimental)
class LEVELSTREAMASSISTANT_API UBlueprintLevelStreamingActorEditorBase : public ULevelStreamingActorEditorBase
{
	GENERATED_BODY()
	
public:
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateSet, AActor*, Target, ULevelStreaming*, LevelStreaming);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateClear, AActor*, Target, ULevelStreaming*, LevelStreaming);
	DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(bool, FDelegateIsSet, const AActor*, Target, const ULevelStreaming*, LevelStreaming);

	virtual void SetBounds(AActor& Target, const FBox& Bounds) const;

	virtual FBox GetBounds(const AActor& Target) const;

	virtual const FSlateBrush* GetDragAndDropActorIcon() const override;

	// change bounds of the target.
	UFUNCTION(BlueprintNativeEvent, Category = "LevelStreamAssistant|Editor", meta = (DisplayName = "SetBounds"))
	void Execute_SetBounds(AActor* Target, const FBox& Bounds) const;
	void Execute_SetBounds_Implementation(AActor* Target, const FBox& Bounds) const;

	// get bounds from the target.
	UFUNCTION(BlueprintNativeEvent, Category = "LevelStreamAssistant|Editor", meta = (DisplayName = "GetBounds"))
	FBox Execute_GetBounds(const AActor* Target) const;
	FBox Execute_GetBounds_Implementation(const AActor* Target) const;

	virtual TSubclassOf<AActor> GetTargetType() const override;

	// Re-Initialize when blueprint is changed or compiled
	virtual void PostInitProperties() override;

	// return the type of actor this editor can work with.
	UFUNCTION(BlueprintNativeEvent, Category = "LevelStreamAssistant|Editor", meta = (DisplayName = "GetTargetType"))
	TSubclassOf<AActor> Execute_GetTargetType() const;
	TSubclassOf<AActor> Execute_GetTargetType_Implementation() const;

	// Define binding modes here.
	UFUNCTION(BlueprintImplementableEvent, Category = "LevelStreamAssistant|Setup")
	void Initialize();

	// Extend the context menu when right clicking on actor items in the hierarchy.
	UFUNCTION(BlueprintImplementableEvent, Category = "LevelStreamAssistant|ContextMenu", meta = (DisplayName = "OnExtendActorsContextMenu"))
	void Execute_OnExtendActorsContextMenu(FLevelStreamAssistant_HierarchyView_ContextMenuBuilder MenuBuilder) const;

	// Extend the context menu when right clicking on level items in the hierarchy.
	UFUNCTION(BlueprintImplementableEvent, Category = "LevelStreamAssistant|ContextMenu", meta = (DisplayName = "OnExtendLevelsContextMenu"))
	void Execute_OnExtendLevelsContextMenu(FLevelStreamAssistant_HierarchyView_ContextMenuBuilder MenuBuilder) const;

	virtual void ExtendActorsContextMenu(FExtender& MenuExtender, TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const override;
	virtual void ExtendLevelsContextMenu(FExtender& MenuExtender, TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const override;

protected:
	// Defines a new binding mode for this editor.
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "LevelStreamAssistant|Setup")
	FLevelStreamBindingMode_Blueprint_Handle AddBindingMode(FDelegateSet Set, FDelegateClear Clear, FDelegateIsSet IsSet, FLinearColor Color, FText ToolTip);
	
	// get current binding mode between target actor and level streaming. (binding mode determines how the target actor streams the level)
	UFUNCTION(BlueprintPure, Category = "LevelStreamAssistant|Editor", meta = (DisplayName = "Get Binding Mode", BlueprintProtected))
	FLevelStreamBindingMode_Blueprint_Handle Execute_GetBinding(const AActor* Target, const ULevelStreaming* LevelStreaming, bool& bIsValid) const;

	// set target actor to stream a level. (e.g. to load or unload the level)
	UFUNCTION(BlueprintCallable, Category = "LevelStreamAssistant|Editor", meta = (DisplayName = "Set Binding Mode", BlueprintProtected))
	void Execute_SetBining(const FLevelStreamBindingMode_Blueprint_Handle& BindingMode, AActor* Target, ULevelStreaming* LevelStreaming);

	UFUNCTION(BlueprintCallable, Category = "LevelStreamAssistant|ContextMenu", meta = (BlueprintProtected))
	void AddMenuItem(UPARAM(ref) FLevelStreamAssistant_HierarchyView_ContextMenuBuilder& MenuBuilder, FText Label, FText ToolTip, FLevelStreamAssistant_HierarchyView_MenuItem_ExecuteAction ExecuteAction)
	{
		MenuBuilder.AddMenuItem(Label, ToolTip, ExecuteAction);
	}
	
	UFUNCTION(BlueprintCallable, Category = "LevelStreamAssistant|Hierarchy", meta = (BlueprintProtected))
	TArray<AActor*> GetLevelStreamingActors(const FLevelStreamAssistantHierarchyView_Blueprint& HierarchyView)
	{
		return HierarchyView.GetLevelStreamingActors();
	}

	UFUNCTION(BlueprintCallable, Category = "LevelStreamAssistant|Hierarchy", meta = (BlueprintProtected))
	TArray<ULevelStreaming*> GetStreamingLevels(const FLevelStreamAssistantHierarchyView_Blueprint& HierarchyView)
	{
		return HierarchyView.GetStreamingLevels();
	}

	UFUNCTION(BlueprintCallable, Category = "LevelStreamAssistant|Hierarchy", meta = (BlueprintProtected))
	TArray<AActor*> GetSelectedLevelStreamingActors(const FLevelStreamAssistantHierarchyView_Blueprint& HierarchyView)
	{
		return HierarchyView.GetSelectedLevelStreamingActors();
	}

	UFUNCTION(BlueprintCallable, Category = "LevelStreamAssistant|Hierarchy", meta = (BlueprintProtected))
	TArray<ULevelStreaming*> GetSelectedStreamingLevels(const FLevelStreamAssistantHierarchyView_Blueprint& HierarchyView)
	{
		return HierarchyView.GetSelectedStreamingLevels();
	}

	UFUNCTION(BlueprintCallable, Category = "LevelStreamAssistant|Hierarchy", meta = (BlueprintProtected))
	void ClearSelectedLevelStreamingActors(UPARAM(ref) FLevelStreamAssistantHierarchyView_Blueprint& HierarchyView)
	{
		HierarchyView.ClearSelectedLevelStreamingActors();
	}

	UFUNCTION(BlueprintCallable, Category = "LevelStreamAssistant|Hierarchy", meta = (BlueprintProtected))
	void ClearSelectedStreamingLevels(UPARAM(ref) FLevelStreamAssistantHierarchyView_Blueprint& HierarchyView)
	{
		HierarchyView.ClearSelectedStreamingLevels();
	}

	UFUNCTION(BlueprintCallable, Category = "LevelStreamAssistant|Hierarchy", meta = (BlueprintProtected))
	void SelectLevelStreamingActor(UPARAM(ref) FLevelStreamAssistantHierarchyView_Blueprint& HierarchyView, AActor* Actor)
	{
		HierarchyView.SelectLevelStreamingActor(Actor);
	}

	UFUNCTION(BlueprintCallable, Category = "LevelStreamAssistant|Hierarchy", meta = (BlueprintProtected))
	void SelectStreamingLevel(UPARAM(ref) FLevelStreamAssistantHierarchyView_Blueprint& HierarchyView, ULevelStreaming* StreamingLevel)
	{
		HierarchyView.SelectStreamingLevel(StreamingLevel);
	}

	UFUNCTION(BlueprintCallable, Category = "LevelStreamAssistant|Hierarchy", meta = (BlueprintProtected))
	void NotifySelectedActorsChanged(UPARAM(ref) FLevelStreamAssistantHierarchyView_Blueprint& HierarchyView)
	{
		HierarchyView.NotifySelectedActorsChanged();
	}

	UFUNCTION(BlueprintCallable, Category = "LevelStreamAssistant|Hierarchy", meta = (BlueprintProtected))
	void NotifySelectedLevelsChanged(UPARAM(ref) FLevelStreamAssistantHierarchyView_Blueprint& HierarchyView)
	{
		HierarchyView.NotifySelectedLevelsChanged();
	}

	UFUNCTION(BlueprintCallable, Category = "LevelStreamAssistant|Hierarchy", meta = (BlueprintProtected))
	void NotifyLevelBindingChanged(UPARAM(ref) FLevelStreamAssistantHierarchyView_Blueprint& HierarchyView)
	{
		HierarchyView.NotifyLevelBindingChanged();
	}

	UFUNCTION(BlueprintPure, Category = "LevelStreamAssistant|Editor", meta = (DisplayName = "IsValid", BlueprintProtected))
	bool BindingModeIsValid(const FLevelStreamBindingMode_Blueprint_Handle& BindingMode) const
	{
		return BindingMode.Get().IsValid();
	}

private:
	void OnExtendActorsContextMenu(FMenuBuilder& MenuBuilder, TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const;
	void OnExtendLevelsContextMenu(FMenuBuilder& MenuBuilder, TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const;
};
