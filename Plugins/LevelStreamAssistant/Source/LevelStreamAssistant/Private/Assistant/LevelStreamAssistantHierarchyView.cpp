// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Assistant/LevelStreamAssistantHierarchyView.h"
#include <Assistant/LevelStreamingEditor.h>
#include <Iterators/WeakPointerIterator.h>

TArray<AActor*> FLevelStreamAssistantHierarchyView::GetLevelStreamingActors() const
{
	TArray<TWeakObjectPtr<AActor>> WeakPtrActors = LevelStreamingEditor->GetLevelStreamingActors();
	TArray<AActor*> Actors;
	Actors.Reserve(WeakPtrActors.Num());
	for (AActor* Actor : TWeakArraySafeRange<AActor>(WeakPtrActors)) Actors.Add(Actor);
	return Actors;
}

TArray<ULevelStreaming*> FLevelStreamAssistantHierarchyView::GetStreamingLevels() const
{
	TArray<TWeakObjectPtr<ULevelStreaming>> WeakPtrStreamingLevels = LevelStreamingEditor->GetStreamingLevels();
	TArray<ULevelStreaming*> StreamingLevels;
	StreamingLevels.Reserve(WeakPtrStreamingLevels.Num());
	for (ULevelStreaming* StreamingLevel : TWeakArraySafeRange<ULevelStreaming>(WeakPtrStreamingLevels)) StreamingLevels.Add(StreamingLevel);
	return StreamingLevels;
}

TArray<AActor*> FLevelStreamAssistantHierarchyView::GetSelectedLevelStreamingActors() const
{
	TArray<TWeakObjectPtr<AActor>> WeakPtrActors = LevelStreamingEditor->GetSelectedLevelStreamingActors();
	TArray<AActor*> Actors;
	Actors.Reserve(WeakPtrActors.Num());
	for (AActor* Actor : TWeakArraySafeRange<AActor>(WeakPtrActors)) Actors.Add(Actor);
	return Actors;
}

TArray<ULevelStreaming*> FLevelStreamAssistantHierarchyView::GetSelectedStreamingLevels() const
{
	TArray<TWeakObjectPtr<ULevelStreaming>> WeakPtrStreamingLevels = LevelStreamingEditor->GetSelectedStreamingLevels();
	TArray<ULevelStreaming*> StreamingLevels;
	StreamingLevels.Reserve(WeakPtrStreamingLevels.Num());
	for (ULevelStreaming* StreamingLevel : TWeakArraySafeRange<ULevelStreaming>(WeakPtrStreamingLevels)) StreamingLevels.Add(StreamingLevel);
	return StreamingLevels;
}

void FLevelStreamAssistantHierarchyView::ClearSelectedLevelStreamingActors()
{
	LevelStreamingEditor->ClearSelectedLevelStreamingActors();
}

void FLevelStreamAssistantHierarchyView::ClearSelectedStreamingLevels()
{
	LevelStreamingEditor->ClearSelectedStreamingLevels();
}

void FLevelStreamAssistantHierarchyView::SelectLevelStreamingActor(AActor* InActor)
{
	LevelStreamingEditor->SelectLevelStreamingActor(InActor);
}

void FLevelStreamAssistantHierarchyView::SelectStreamingLevel(ULevelStreaming* InStreamingLevel)
{
	LevelStreamingEditor->SelectStreamingLevel(InStreamingLevel);
}

void FLevelStreamAssistantHierarchyView::NotifySelectedActorsChanged() const
{
	LevelStreamingEditor->NotifySelectedActorsChanged();
}

void FLevelStreamAssistantHierarchyView::NotifySelectedLevelsChanged() const
{
	LevelStreamingEditor->NotifySelectedLevelsChanged();
}

void FLevelStreamAssistantHierarchyView::NotifyLevelBindingChanged() const
{
	LevelStreamingEditor->NotifyLevelBindingChanged();
}
