// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include <Assistant/DragAndDrop/ItemDragAndDropHandlers.h>
#include <Assistant/DragAndDrop/DragDropValidationVisitor.h>
#include <Assistant/DragAndDrop/DragDropOpVisitor.h>
#include <Assistant/DragAndDrop/TreeItemDragDropOp.h>
#include <Assistant/LevelStreamingEditor.h>
#include <Widgets/Views/SListView.h>
#include <Assistant/TreeItems.h>

#define LOCTEXT_NAMESPACE "LevelStreamAssistant.ItemDragAndDropHandlers"

namespace LevelStreamAssistant
{
	FReply OnItemDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, TWeakPtr<SListView<TSharedPtr<FBaseItem>>> ListView)
	{
		if (!ListView.IsValid()) FReply::Unhandled();

		if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
		{
			const TArray<TSharedPtr<FBaseItem>>& ItemsToDrag = ListView.Pin()->GetSelectedItems();
			return FReply::Handled().BeginDragDrop(FTreeItemDragDropOp::New(ItemsToDrag));
		}

		return FReply::Unhandled();
	}

	void OnItemDragLeave(FDragDropEvent const& DragDropEvent)
	{
		TSharedPtr<FTreeItemDragDropOp> DragDropOp = DragDropEvent.GetOperationAs<FTreeItemDragDropOp>();
		if (DragDropOp.IsValid())
		{
			DragDropOp->ResetToDefaultToolTip();
		}
	}

	TOptional<EItemDropZone> OnItemCanAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, TSharedPtr<FBaseItem> DropTarget)
	{
		if (DropTarget.IsValid())
		{
			TSharedPtr<FTreeItemDragDropOp> DragDropOp = DragDropEvent.GetOperationAs<FTreeItemDragDropOp>();
			if (DragDropOp.IsValid())
			{
				const TArray<TSharedPtr<FBaseItem>>& DraggedItems = DragDropOp->ItemsToDrop;

				if (DraggedItems.Num() > 0)
				{
					FDragDropValidationVisitor Validator(DropTarget);

					// check if drag and drop is valid on this item.
					for (TSharedPtr<FBaseItem> DraggedItem : DraggedItems)
					{
						if (DraggedItem.IsValid())
						{
							DraggedItem->Accept(Validator);

							// if drag and drop is not valid on this item.
							if (!Validator.bCanDrop)
							{
								if (Validator.bShowToolTip)
								{
									DragDropOp->SetToolTip(Validator.Tooltip, Validator.Icon);
								}
								return TOptional<EItemDropZone>();
							}
						}
					}

					if (Validator.bShowToolTip)
					{
						if (DraggedItems.Num() > 1)
						{
							// set tooltip for drag and dropping many items.
							FText Tooltip = FText::FromString(FString::Printf(TEXT("Move %i items to selection"), DraggedItems.Num()));
							DragDropOp->SetToolTip(Tooltip, Validator.Icon);
						}
						else
						{
							// set appropriate tooltip for drag and dropping single item.
							DragDropOp->SetToolTip(Validator.Tooltip, Validator.Icon);
						}
					}

					return EItemDropZone::OntoItem; // accept drop
				}
			}
		}
		return TOptional<EItemDropZone>(); // reject drop
	}

	FReply OnItemAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, TSharedPtr<FBaseItem> DropTarget)
	{
		if (DropTarget.IsValid())
		{
			TSharedPtr<FTreeItemDragDropOp> DragDropOp = DragDropEvent.GetOperationAs<FTreeItemDragDropOp>();
			if (DragDropOp.IsValid())
			{
				const TArray<TSharedPtr<FBaseItem>>& DraggedItems = DragDropOp->ItemsToDrop;

				if (DraggedItems.Num() > 0)
				{
					FDragDropOpVisitor DropOnTarget(DropTarget);

					bool bDraggingMultipleItems = DraggedItems.Num() > 1;
					FScopedTransaction Transaction(LOCTEXT("TransactionDragDropItems", "Move Items"), bDraggingMultipleItems);

					for (TSharedPtr<FBaseItem> DraggedItem : DraggedItems)
					{
						if (DraggedItem.IsValid())
						{
							DraggedItem->Accept(DropOnTarget);
						}
					}
				}
			}
		}
		return FReply::Handled();
	}
}

#undef LOCTEXT_NAMESPACE
