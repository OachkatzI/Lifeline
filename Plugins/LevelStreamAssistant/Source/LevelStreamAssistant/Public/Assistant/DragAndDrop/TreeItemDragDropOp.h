// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <DragAndDrop/DecoratedDragDropOp.h>

namespace LevelStreamAssistant
{
	struct FBaseItem;

	class FTreeItemDragDropOp : public FDecoratedDragDropOp
	{
	public:
		DRAG_DROP_OPERATOR_TYPE(FTreeItemDragDropOp, FDecoratedDragDropOp)

		TArray<TSharedPtr<FBaseItem>> ItemsToDrop;

		/** Inits the tooltip */
		void Init();

		static TSharedRef<FTreeItemDragDropOp> New(const TArray<TSharedPtr<FBaseItem>>& InItemsToDrop);
	};
}
