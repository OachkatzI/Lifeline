// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "BlueprintLevelStreamingActorEditorBase.h"
#include "LevelStreamBindingMode.h"
#include <LevelStreamAssistantStyle.h>
#include <Assistant/LevelStreamAssistantHierarchyView.h>

class FLevelStreamBindingMode_Blueprint : public FLevelStreamBindingMode
{
private:
	typedef UBlueprintLevelStreamingActorEditorBase::FDelegateSet FDelegateSet;
	typedef UBlueprintLevelStreamingActorEditorBase::FDelegateClear FDelegateClear;
	typedef UBlueprintLevelStreamingActorEditorBase::FDelegateIsSet FDelegateIsSet;

	FDelegateSet SetDelegate;
	FDelegateClear ClearDelegate;
	FDelegateIsSet IsSetDelegate;
	FLinearColor Color;
	FText ToolTip;

public:
	virtual ~FLevelStreamBindingMode_Blueprint() { }

	FLevelStreamBindingMode_Blueprint(FDelegateSet InSetDelegate, FDelegateClear InClearDelegate, FDelegateIsSet InIsSetDelegate, FLinearColor InColor, FText InToolTip)
		: SetDelegate(InSetDelegate), ClearDelegate(InClearDelegate), IsSetDelegate(InIsSetDelegate), Color(InColor), ToolTip(InToolTip) { }

	virtual void Set(AActor& Target, ULevelStreaming& LevelStreaming) const override
	{
		SetDelegate.ExecuteIfBound(&Target, &LevelStreaming);
	}
	
	virtual void Clear(AActor& Target, ULevelStreaming& LevelStreaming) const override
	{
		ClearDelegate.ExecuteIfBound(&Target, &LevelStreaming);
	}

	virtual bool IsSet(const AActor& Target, const ULevelStreaming& LevelStreaming) const override
	{
		return IsSetDelegate.IsBound() && IsSetDelegate.Execute(&Target, &LevelStreaming);
	}

	virtual FText GetToolTip() const override { return ToolTip; }

	virtual FLinearColor GetColor() const override { return Color; }
};

TSubclassOf<AActor> UBlueprintLevelStreamingActorEditorBase::GetTargetType() const
{
	return Execute_GetTargetType();
}

void UBlueprintLevelStreamingActorEditorBase::PostInitProperties()
{
	Super::PostInitProperties();

	BindingModes.Empty();
	Initialize();
}

void UBlueprintLevelStreamingActorEditorBase::SetBounds(AActor& Target, const FBox& Bounds) const
{
	Execute_SetBounds(&Target, Bounds);
}

FBox UBlueprintLevelStreamingActorEditorBase::GetBounds(const AActor& Target) const
{
	return Execute_GetBounds(&Target);
}

const FSlateBrush* UBlueprintLevelStreamingActorEditorBase::GetDragAndDropActorIcon() const
{
	return FLevelStreamAssistantStyle::Get().GetBrush("LSA.Level.Zone.Blue");
}

void UBlueprintLevelStreamingActorEditorBase::Execute_SetBounds_Implementation(AActor* Target, const FBox& Bounds) const
{
	if (IsActorSupported(Target)) Super::SetBounds(*Target, Bounds);
}

FBox UBlueprintLevelStreamingActorEditorBase::Execute_GetBounds_Implementation(const AActor* Target) const
{
	return IsActorSupported(Target) ? Super::GetBounds(*Target) : FBox(ForceInit);
}

TSubclassOf<AActor> UBlueprintLevelStreamingActorEditorBase::Execute_GetTargetType_Implementation() const
{
	return AActor::StaticClass();
}

FLevelStreamBindingMode_Blueprint_Handle UBlueprintLevelStreamingActorEditorBase::AddBindingMode(FDelegateSet Set, FDelegateClear Clear, FDelegateIsSet IsSet, FLinearColor Color, FText ToolTip)
{
	TSharedPtr<FLevelStreamBindingMode_Blueprint> BindingMode = MakeShareable(new FLevelStreamBindingMode_Blueprint(Set, Clear, IsSet, Color, ToolTip));
	BindingModes.Add(BindingMode);
	return FLevelStreamBindingMode_Blueprint_Handle(BindingMode);
}

FLevelStreamBindingMode_Blueprint_Handle UBlueprintLevelStreamingActorEditorBase::Execute_GetBinding(const AActor* Target, const ULevelStreaming* LevelStreaming, bool& bIsValid) const
{
	TSharedPtr<FLevelStreamBindingMode> BindingMode = GetBinding(Target, LevelStreaming);
	bIsValid = BindingMode.IsValid();
	return FLevelStreamBindingMode_Blueprint_Handle(BindingMode);
}

void UBlueprintLevelStreamingActorEditorBase::Execute_SetBining(const FLevelStreamBindingMode_Blueprint_Handle& BindingMode, AActor* Target, ULevelStreaming* LevelStreaming)
{
	SetBinding(Target, LevelStreaming, BindingMode.Get());
}

void UBlueprintLevelStreamingActorEditorBase::ExtendActorsContextMenu(FExtender& MenuExtender, TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const
{
	Super::ExtendActorsContextMenu(MenuExtender, HierarchyView);

	MenuExtender.AddMenuExtension("Actor", EExtensionHook::After, nullptr, FMenuExtensionDelegate::CreateUObject(this, &UBlueprintLevelStreamingActorEditorBase::OnExtendActorsContextMenu, HierarchyView));
}

void UBlueprintLevelStreamingActorEditorBase::ExtendLevelsContextMenu(FExtender& MenuExtender, TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const
{
	Super::ExtendLevelsContextMenu(MenuExtender, HierarchyView);

	MenuExtender.AddMenuExtension("Level", EExtensionHook::After, nullptr, FMenuExtensionDelegate::CreateUObject(this, &UBlueprintLevelStreamingActorEditorBase::OnExtendLevelsContextMenu, HierarchyView));
}

void UBlueprintLevelStreamingActorEditorBase::OnExtendActorsContextMenu(FMenuBuilder& MenuBuilder, TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const
{
	FLevelStreamAssistantHierarchyView_Blueprint HierarchyView_Blueprint(HierarchyView);
	FLevelStreamAssistant_HierarchyView_ContextMenuBuilder MenuBuilder_Blueprint(MenuBuilder, HierarchyView_Blueprint);
	Execute_OnExtendActorsContextMenu(MenuBuilder_Blueprint);
}

void UBlueprintLevelStreamingActorEditorBase::OnExtendLevelsContextMenu(FMenuBuilder& MenuBuilder, TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView) const
{
	FLevelStreamAssistantHierarchyView_Blueprint HierarchyView_Blueprint(HierarchyView);
	FLevelStreamAssistant_HierarchyView_ContextMenuBuilder MenuBuilder_Blueprint(MenuBuilder, HierarchyView_Blueprint);
	Execute_OnExtendLevelsContextMenu(MenuBuilder_Blueprint);
}

TArray<AActor*> FLevelStreamAssistantHierarchyView_Blueprint::GetLevelStreamingActors() const
{
	return HierarchyView.IsValid() ? HierarchyView->GetLevelStreamingActors() : TArray<AActor*>();
}

TArray<ULevelStreaming*> FLevelStreamAssistantHierarchyView_Blueprint::GetStreamingLevels() const
{
	return HierarchyView.IsValid() ? HierarchyView->GetStreamingLevels() : TArray<ULevelStreaming*>();
}

TArray<AActor*> FLevelStreamAssistantHierarchyView_Blueprint::GetSelectedLevelStreamingActors() const
{
	return HierarchyView.IsValid() ? HierarchyView->GetSelectedLevelStreamingActors() : TArray<AActor*>();
}

TArray<ULevelStreaming*> FLevelStreamAssistantHierarchyView_Blueprint::GetSelectedStreamingLevels() const
{
	return HierarchyView.IsValid() ? HierarchyView->GetSelectedStreamingLevels() : TArray<ULevelStreaming*>();
}

void FLevelStreamAssistantHierarchyView_Blueprint::ClearSelectedLevelStreamingActors()
{
	if (HierarchyView.IsValid()) HierarchyView->ClearSelectedLevelStreamingActors();
}

void FLevelStreamAssistantHierarchyView_Blueprint::ClearSelectedStreamingLevels()
{
	if (HierarchyView.IsValid()) HierarchyView->ClearSelectedStreamingLevels();
}

void FLevelStreamAssistantHierarchyView_Blueprint::SelectLevelStreamingActor(AActor* InActor)
{
	if (HierarchyView.IsValid()) HierarchyView->SelectLevelStreamingActor(InActor);
}

void FLevelStreamAssistantHierarchyView_Blueprint::SelectStreamingLevel(ULevelStreaming* InStreamingLevel)
{
	if (HierarchyView.IsValid()) HierarchyView->SelectStreamingLevel(InStreamingLevel);
}

void FLevelStreamAssistantHierarchyView_Blueprint::NotifySelectedActorsChanged() const
{
	if (HierarchyView.IsValid()) HierarchyView->NotifySelectedActorsChanged();
}

void FLevelStreamAssistantHierarchyView_Blueprint::NotifySelectedLevelsChanged() const
{
	if (HierarchyView.IsValid()) HierarchyView->NotifySelectedLevelsChanged();
}

void FLevelStreamAssistantHierarchyView_Blueprint::NotifyLevelBindingChanged() const
{
	if (HierarchyView.IsValid()) HierarchyView->NotifyLevelBindingChanged();
}

void FLevelStreamAssistant_HierarchyView_ContextMenuBuilder::AddMenuItem(FText Label, FText ToolTip, FLevelStreamAssistant_HierarchyView_MenuItem_ExecuteAction ExecuteAction)
{
	if (!MenuBuilder) return;

	MenuBuilder->AddMenuEntry(
		Label, ToolTip, FSlateIcon(),
		FUIAction(FExecuteAction::CreateLambda([HierarchyView = HierarchyView, ExecuteAction]()
		{
			ExecuteAction.ExecuteIfBound(HierarchyView);
		}))
	);
}