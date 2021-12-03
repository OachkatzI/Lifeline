// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class ULevelStreaming;
class AActor;
class FLevelStreamBindingMode;
class ULevelStreamingActorEditorBase;

namespace LevelStreamAssistant 
{
	// interface for editing level streaming.
	class ILevelStreamingEditor
	{
	public:
		virtual ~ILevelStreamingEditor() { }

		virtual ULevelStreamingActorEditorBase& GetLevelStreamingActorEditor() const = 0;
		virtual void SetLevelStreamingActorEditorType(TSubclassOf<ULevelStreamingActorEditorBase> InEditorType) = 0;

		virtual bool IsSupportedByEditor(const AActor* Actor) const = 0;

		virtual const TArray<TSharedPtr<FLevelStreamBindingMode>>& GetBindingModes() const = 0;

		virtual const TArray<TWeakObjectPtr<AActor>>& GetLevelStreamingActors() const = 0;
		virtual void SetLevelStreamingActors(const TArray<TWeakObjectPtr<AActor>>& InActors) = 0;

		virtual const TArray<TWeakObjectPtr<ULevelStreaming>>& GetStreamingLevels() const = 0;
		virtual void SetStreamingLevels(const TArray<TWeakObjectPtr<ULevelStreaming>>& InStreamingLevels) = 0;

		virtual const TArray<TWeakObjectPtr<AActor>>& GetSelectedLevelStreamingActors() const = 0;
		virtual void ClearSelectedLevelStreamingActors() = 0;
		virtual void SelectLevelStreamingActor(const TWeakObjectPtr<AActor>& InActorToSelect) = 0;

		virtual const TArray<TWeakObjectPtr<ULevelStreaming>>& GetSelectedStreamingLevels() const = 0;
		virtual void ClearSelectedStreamingLevels() = 0;
		virtual void SelectStreamingLevel(const TWeakObjectPtr<ULevelStreaming>& InLevelToSelect) = 0;

		virtual TSharedPtr<FLevelStreamBindingMode> GetBinding(const AActor* InActor, const ULevelStreaming* InLevel) const = 0;
		
		virtual void SetBinding(AActor* InActor, ULevelStreaming* InLevel, TSharedPtr<FLevelStreamBindingMode> NewBindingMode) const = 0;

		void NotifyLevelBindingChanged() const;
		FSimpleMulticastDelegate& OnLevelBindingChanged();

		void NotifySelectedActorsChanged() const;
		FSimpleMulticastDelegate& OnSelectedActorsChanged();

		void NotifySelectedLevelsChanged() const;
		FSimpleMulticastDelegate& OnSelectedLevelsChanged();

	private:
		FSimpleMulticastDelegate OnLevelBindingChangedEvent;
		FSimpleMulticastDelegate OnSelectedActorsChangedEvent;
		FSimpleMulticastDelegate OnSelectedLevelsChangedEvent;
	};

	class FLevelStreamingEditorImpl : public ILevelStreamingEditor
	{
	public:
		FLevelStreamingEditorImpl(TSubclassOf<ULevelStreamingActorEditorBase> InEditorType) : EditorType(InEditorType) { }

		virtual ~FLevelStreamingEditorImpl() override { }

		virtual const TArray<TSharedPtr<FLevelStreamBindingMode>>& GetBindingModes() const override;

		virtual ULevelStreamingActorEditorBase& GetLevelStreamingActorEditor() const override;
		virtual void SetLevelStreamingActorEditorType(TSubclassOf<ULevelStreamingActorEditorBase> InEditorType) override;

		virtual bool IsSupportedByEditor(const AActor* Actor) const override;

		virtual const TArray<TWeakObjectPtr<AActor>>& GetLevelStreamingActors() const override;
		virtual void SetLevelStreamingActors(const TArray<TWeakObjectPtr<AActor>>& InActors) override;

		virtual const TArray<TWeakObjectPtr<ULevelStreaming>>& GetStreamingLevels() const override;
		virtual void SetStreamingLevels(const TArray<TWeakObjectPtr<ULevelStreaming>>& InStreamingLevels) override;

		virtual const TArray<TWeakObjectPtr<AActor>>& GetSelectedLevelStreamingActors() const override;
		virtual void ClearSelectedLevelStreamingActors() override;
		virtual void SelectLevelStreamingActor(const TWeakObjectPtr<AActor>& InActorToSelect) override;

		virtual const TArray<TWeakObjectPtr<ULevelStreaming>>& GetSelectedStreamingLevels() const override;
		virtual void ClearSelectedStreamingLevels() override;
		virtual void SelectStreamingLevel(const TWeakObjectPtr<ULevelStreaming>& InLevelToSelect) override;

		virtual TSharedPtr<FLevelStreamBindingMode> GetBinding(const AActor* InActor, const ULevelStreaming* InLevel) const;
		
		virtual void SetBinding(AActor* InActor, ULevelStreaming* InLevel, TSharedPtr<FLevelStreamBindingMode> NewBindingMode) const override;

	private:
		mutable TWeakObjectPtr<ULevelStreamingActorEditorBase> CachedEditorPtr;
		mutable TSoftClassPtr<ULevelStreamingActorEditorBase> EditorType;

		TArray<TWeakObjectPtr<AActor>> LevelStreamingActors;
		TArray<TWeakObjectPtr<ULevelStreaming>> StreamingLevels;

		TArray<TWeakObjectPtr<AActor>> SelectedLevelStreamingActors;
		TArray<TWeakObjectPtr<ULevelStreaming>> SelectedStreamingLevels;
	};
}
