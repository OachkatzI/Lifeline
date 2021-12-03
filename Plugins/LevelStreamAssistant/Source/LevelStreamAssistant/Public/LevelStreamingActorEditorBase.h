// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LevelStreamingActorEditorBase.generated.h"

class AActor;
class ULevelStreaming;
class FLevelStreamBindingMode;
class FMenuBuilder;
class FExtender;
struct FLevelStreamAssistantHierarchyView;

// manage level streaming with actors. this class should remain stateless.
UCLASS(Abstract, Transient, Experimental, PerObjectConfig, Config = EditorPerProjectUserSettings)
class LEVELSTREAMASSISTANT_API ULevelStreamingActorEditorBase : public UObject
{
	GENERATED_BODY()

public:
	// change bounds of the target.
	virtual void SetBounds(AActor& Target, const FBox& Bounds) const;

	// get bounds from the target.
	virtual FBox GetBounds(const AActor& Target) const;

	// called when right clicking on selected actors in the actor hierarchy.
	virtual void ExtendActorsContextMenu(FExtender& MenuExtender, TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const;
	
	// called when right clicking on selected levels in the level hierarchy.
	virtual void ExtendLevelsContextMenu(FExtender& MenuExtender, TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const { }

	// return the type of actor this editor can work with.
	virtual TSubclassOf<AActor> GetTargetType() const;

	virtual const FSlateBrush* GetDragAndDropActorIcon() const;

	virtual bool IsEditorOnly() const override final { return true; }

	const TArray<TSharedPtr<FLevelStreamBindingMode>>& GetBindingModes() const { return BindingModes; }

	void SetBounds(AActor* Target, const FBox& Bounds) const;
	FBox GetBounds(const AActor* Target) const;

	bool IsActorSupported(const AActor* Actor) const;
	TSharedPtr<FLevelStreamBindingMode> GetBinding(const AActor* InActor, const ULevelStreaming* InLevel) const;
	void SetBinding(AActor* InActor, ULevelStreaming* InLevel, const TSharedPtr<FLevelStreamBindingMode>& NewBindingMode) const;

	// This color is used for highlighting the selected level streaming actor.
	UPROPERTY(EditAnywhere, Config, Category = "Highlight Settings")
	FLinearColor SelectedActorColor = FLinearColor(0.05f, 0.05f, 1.f, 0.3f);

protected:

	TArray<TSharedPtr<FLevelStreamBindingMode>> BindingModes;

	template<typename TargetType>
	static TargetType& Get(AActor& Target)
	{
		TargetType* CastedTarget = Cast<TargetType>(&Target);
		check(CastedTarget);
		return *CastedTarget;
	}

	template<typename TargetType>
	static const TargetType& Get(const AActor& Target)
	{
		const TargetType* CastedTarget = Cast<TargetType>(&Target);
		check(CastedTarget);
		return *CastedTarget;
	}

private:
	void OnExtendActorsContextMenu(FMenuBuilder& MenuBuilder, TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const;

	void ExecuteAction_PlayFromHere(AActor* SelectedActor) const;
	
	void ExecuteAction_BindAll(TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const;
	void ExecuteAction_UnbindAll(TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const;

	void ExecuteAction_CalculateBoundFromLevels(TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const;
	void ExecuteAction_CalculateBoundFromActors(TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const;
	void ExecuteAction_SelectActorsInside(TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const;
};
