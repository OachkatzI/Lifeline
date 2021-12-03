// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Templates/SharedPointer.h>

template<typename ItemType> class SListView;
class FReply;
struct FGeometry;
struct FPointerEvent;
class FDragDropEvent;
enum class EItemDropZone;

namespace LevelStreamAssistant
{
	struct FBaseItem;

	FReply OnItemDragDetected(const FGeometry& Geometry, const FPointerEvent& PointerEvent, TWeakPtr<SListView<TSharedPtr<FBaseItem>>> ItemsListView);
	void OnItemDragLeave(FDragDropEvent const& DragDropEvent);
	TOptional<EItemDropZone> OnItemCanAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, TSharedPtr<FBaseItem> DropTarget);
	FReply OnItemAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, TSharedPtr<FBaseItem> DropTarget);
}
