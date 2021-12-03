// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include <Assistant/LevelStreamingEditor.h>
#include "LevelStreamingVolumeEditor.h"

namespace LevelStreamAssistant
{
	void ILevelStreamingEditor::NotifyLevelBindingChanged() const
	{
		OnLevelBindingChangedEvent.Broadcast();
	}

	FSimpleMulticastDelegate& ILevelStreamingEditor::OnLevelBindingChanged()
	{
		return OnLevelBindingChangedEvent;
	}

	void ILevelStreamingEditor::NotifySelectedActorsChanged() const
	{
		OnSelectedActorsChangedEvent.Broadcast();
	}

	FSimpleMulticastDelegate& ILevelStreamingEditor::OnSelectedActorsChanged()
	{
		return OnSelectedActorsChangedEvent;
	}

	void ILevelStreamingEditor::NotifySelectedLevelsChanged() const
	{
		OnSelectedLevelsChangedEvent.Broadcast();
	}

	FSimpleMulticastDelegate& ILevelStreamingEditor::OnSelectedLevelsChanged()
	{
		return OnSelectedLevelsChangedEvent;
	}

	const TArray<TSharedPtr<FLevelStreamBindingMode>>& FLevelStreamingEditorImpl::GetBindingModes() const
	{
		return GetLevelStreamingActorEditor().GetBindingModes();
	}

	ULevelStreamingActorEditorBase& FLevelStreamingEditorImpl::GetLevelStreamingActorEditor() const
	{
		if (CachedEditorPtr.IsValid()) return *CachedEditorPtr;

		UClass* EditorTypePtr = EditorType.Get();

		if (!EditorTypePtr || !ensure(!EditorTypePtr->HasAnyClassFlags(EClassFlags::CLASS_Abstract)))
		{
			EditorTypePtr = ULevelStreamingVolumeEditor::StaticClass(); // fallback value.
			EditorType = EditorTypePtr;
		}

		FString EditorName = FString::Printf(TEXT("%s_Preset_01"), *EditorTypePtr->GetName());
		UObject* FoundEditor = StaticFindObject(EditorTypePtr, GetTransientPackage(), *EditorName, /*ExactClass*/true);
		CachedEditorPtr = Cast<ULevelStreamingActorEditorBase>(FoundEditor);

		if (CachedEditorPtr.IsValid()) return *CachedEditorPtr;

		ULevelStreamingActorEditorBase* NewEditor = NewObject<ULevelStreamingActorEditorBase>(GetTransientPackage(), EditorTypePtr, *EditorName, RF_Transient);
		if (NewEditor) NewEditor->LoadConfig();
		CachedEditorPtr = NewEditor;
		check(CachedEditorPtr.IsValid());
		return *CachedEditorPtr;
	}

	void FLevelStreamingEditorImpl::SetLevelStreamingActorEditorType(TSubclassOf<ULevelStreamingActorEditorBase> InEditorType)
	{
		if (EditorType.Get() != InEditorType)
		{
			EditorType = InEditorType;
			CachedEditorPtr = nullptr;
		}
	}

	bool FLevelStreamingEditorImpl::IsSupportedByEditor(const AActor* Actor) const
	{
		return GetLevelStreamingActorEditor().IsActorSupported(Actor);
	}

	const TArray<TWeakObjectPtr<AActor>>& FLevelStreamingEditorImpl::GetLevelStreamingActors() const
	{
		return LevelStreamingActors;
	}

	void FLevelStreamingEditorImpl::SetLevelStreamingActors(const TArray<TWeakObjectPtr<AActor>>& InActors)
	{
		LevelStreamingActors = InActors;
	}

	const TArray<TWeakObjectPtr<ULevelStreaming>>& FLevelStreamingEditorImpl::GetStreamingLevels() const
	{
		return StreamingLevels;
	}

	void FLevelStreamingEditorImpl::SetStreamingLevels(const TArray<TWeakObjectPtr<ULevelStreaming>>& InStreamingLevels)
	{
		StreamingLevels = InStreamingLevels;
	}

	const TArray<TWeakObjectPtr<AActor>>& FLevelStreamingEditorImpl::GetSelectedLevelStreamingActors() const
	{
		return SelectedLevelStreamingActors;
	}

	void FLevelStreamingEditorImpl::ClearSelectedLevelStreamingActors()
	{
		SelectedLevelStreamingActors.Empty();
	}

	void FLevelStreamingEditorImpl::SelectLevelStreamingActor(const TWeakObjectPtr<AActor>& InActorToSelect)
	{
		if (IsSupportedByEditor(InActorToSelect.Get()))
		{
			SelectedLevelStreamingActors.Add(InActorToSelect);
		}
	}

	const TArray<TWeakObjectPtr<ULevelStreaming>>& FLevelStreamingEditorImpl::GetSelectedStreamingLevels() const
	{
		return SelectedStreamingLevels;
	}

	void FLevelStreamingEditorImpl::ClearSelectedStreamingLevels()
	{
		SelectedStreamingLevels.Empty();
	}

	void FLevelStreamingEditorImpl::SelectStreamingLevel(const TWeakObjectPtr<ULevelStreaming>& InLevelToSelect)
	{
		if (InLevelToSelect.IsValid())
		{
			SelectedStreamingLevels.Add(InLevelToSelect);
		}
	}

	TSharedPtr<FLevelStreamBindingMode> FLevelStreamingEditorImpl::GetBinding(const AActor* InActor, const ULevelStreaming* InLevel) const
	{
		return GetLevelStreamingActorEditor().GetBinding(InActor, InLevel);
	}

	void FLevelStreamingEditorImpl::SetBinding(AActor* InActor, ULevelStreaming* InLevel, TSharedPtr<FLevelStreamBindingMode> NewBindingMode) const
	{
		GetLevelStreamingActorEditor().SetBinding(InActor, InLevel, NewBindingMode);
	}
}
