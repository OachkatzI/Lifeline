// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Assistant/STreeViewBase.h" 
#include <Widgets/STreeViewWithColumn.h>
#include <Assistant/TreeItems.h>
#include <Iterators/ItemIterators.h>
#include <Assistant/DragAndDrop/DragDropValidationVisitor.h>
#include <Assistant/DragAndDrop/DragDropOpVisitor.h>
#include <Assistant/DragAndDrop/TreeItemDragDropOp.h>
#include <Assistant/Folders/FolderManager.h>
#include <Utilities/LevelStreamAssistantItemUtils.h>
#include <Utilities/LevelStreamAssistantUtils.h>
#include <Widgets/Colors/SColorPicker.h>
#include <Editor.h>

#define LOCTEXT_NAMESPACE "LevelStreamAssistant.STreeViewBase"

namespace LevelStreamAssistant
{
	void STreeViewBase::RequestTreeRefresh()
	{
		if (TreeView.IsValid())
		{
			UpdateItemsExpansion();
			TreeView->RequestTreeRefresh();
		}

		HandlePendingRenameAfterCreateFolder();
	}

	void STreeViewBase::RebuildTree()
	{
		if (TreeView.IsValid())
		{
			UpdateItemsExpansion();
			TreeView->RebuildList();
		}

		HandlePendingRenameAfterCreateFolder();
	}

	void STreeViewBase::ClearSelection()
	{
		if (TreeView.IsValid()) TreeView->ClearSelection();
	}

	void STreeViewBase::SetItemSelection(const TSharedPtr<FBaseItem>& InItem, bool bSelected)
	{
		if (TreeView.IsValid()) TreeView->SetItemSelection(InItem, bSelected);
	}

	void STreeViewBase::RequestScrollIntoView(const TSharedPtr<FBaseItem>& InItem)
	{
		if (TreeView.IsValid()) TreeView->RequestScrollIntoView(InItem);
	}

	TArray<TSharedPtr<FBaseItem>> STreeViewBase::GetSelectedItems() const
	{
		TArray<TSharedPtr<FBaseItem>> SelectedItems;
		if (TreeView.IsValid())
		{
			TreeView->GetSelectedItems(SelectedItems);
		}
		return SelectedItems;
	}

	int32 STreeViewBase::GetNumSelectedItems() const
	{
		return TreeView.IsValid() ? TreeView->GetNumItemsSelected() : 0;
	}

	void STreeViewBase::OnGetRowChildren(TSharedPtr<FBaseItem> InItem, TArray<TSharedPtr<FBaseItem>>& OutChildren)
	{
		if (InItem.IsValid()) OutChildren = InItem->Children;
	}

	FReply STreeViewBase::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
	{
		if (CommandList && CommandList->ProcessCommandBindings(InKeyEvent))
		{
			return FReply::Handled();
		}
		return FReply::Unhandled();
	}

	void STreeViewBase::OnItemExpansionChanged(TSharedPtr<FBaseItem> Item, bool bExpanded) const
	{
		if (Item.IsValid())
		{
			Item->SetExpansion(bExpanded);
		}
	}

	void STreeViewBase::UpdateItemsExpansion()
	{
		if (!TreeView.IsValid() || !TreeItemsSource) return;

		for (TSharedPtr<FBaseItem> Item : FItemHierarchyRange(*TreeItemsSource))
		{
			if (Item.IsValid())
			{
				TreeView->SetItemExpansion(Item, Item->IsExpanded());
			}
		}
	}

	FReply STreeViewBase::OnItemDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
	{
		if (!TreeView.IsValid()) FReply::Unhandled();

		if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
		{
			const TArray<TSharedPtr<FBaseItem>>& ItemsToDrag = TreeView->GetSelectedItems();
			return FReply::Handled().BeginDragDrop(FTreeItemDragDropOp::New(ItemsToDrag));
		}

		return FReply::Unhandled();
	}

	void STreeViewBase::OnItemDragLeave(FDragDropEvent const& DragDropEvent)
	{
		TSharedPtr<FTreeItemDragDropOp> DragDropOp = DragDropEvent.GetOperationAs<FTreeItemDragDropOp>();
		if (DragDropOp.IsValid())
		{
			DragDropOp->ResetToDefaultToolTip();
		}
	}

	TOptional<EItemDropZone> STreeViewBase::OnItemCanAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, TSharedPtr<FBaseItem> DropTarget)
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

	FReply STreeViewBase::OnItemAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, TSharedPtr<FBaseItem> DropTarget)
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

	void STreeViewBase::CreateFolderForSelectedItems()
	{
		FFolderManager& FolderManager = GetFolderManager();
		TArray<TSharedPtr<FBaseItem>> SelectedItems = GetSelectedItems();
		FName NewFolder = LevelStreamAssistantUtils::CreateFolder(FolderManager, SelectedItems);

		MakeFolderRenameRequest(NewFolder);
	}

	void STreeViewBase::CreateSubFolderForSelectedItem()
	{
		if (GetNumSelectedItems() == 1)
		{
			if (TSharedPtr<FBaseItem> SelectedItem = GetSelectedItems()[0])
			{
				FName FolderPath = NAME_None;
				SelectedItem->Accept(FFunctionalItemVisitor()
					.ActorFolder([&FolderPath](const FActorFolderItem& Folder) { FolderPath = Folder.Path; })
					.LevelFolder([&FolderPath](const FLevelFolderItem& Folder) { FolderPath = Folder.Path; })
				);

				if (UWorld* World = LevelStreamAssistantUtils::GetWorldFromEditor())
				{
					FFolderManager& FolderManager = GetFolderManager();
					FName SubFolderName = FolderManager.GetUniqueFolderName(*World, FolderPath);

					FScopedTransaction Transaction(LOCTEXT("TransactionCreateSubFolder", "Create Sub Folder"));

					FolderManager.CreateFolder(*World, SubFolderName);

					MakeFolderRenameRequest(SubFolderName);
				}
			}
		}
	}

	void STreeViewBase::MakeFolderRenameRequest(FName InFolderToRename)
	{
		FolderToRename = InFolderToRename;

		// wait for SLevelStreamAssistantWidget to respond to CreateFolder event and refresh this tree. then a request is made to rename the created folder.
		bPendingRenameAfterCreateFolder = true;
	}

	void STreeViewBase::HandlePendingRenameAfterCreateFolder()
	{
		if (bPendingRenameAfterCreateFolder)
		{
			// FBaseItem for new folder may not be generated yet. so request to rename FBaseItem is performed on next tick.
			RegisterActiveTimer(0.0f, FWidgetActiveTimerDelegate::CreateSP(this, &STreeViewBase::RequestToRenameCreatedFolder, FolderToRename));

			FolderToRename = NAME_None;
			bPendingRenameAfterCreateFolder = false;
		}
	}
	
	EActiveTimerReturnType STreeViewBase::RequestToRenameCreatedFolder(double CurrentTime, float DeltaTime, FName InFolderToRename)
	{
		if (!TreeItemsSource || !TreeView.IsValid()) return EActiveTimerReturnType::Stop;

		for (TSharedPtr<FBaseItem> Item : FItemHierarchyRange(*TreeItemsSource))
		{
			if (Item.IsValid())
			{
				bool bShouldRenameItem = false;

				Item->Accept(FFunctionalItemVisitor()
					.LevelFolder([&bShouldRenameItem, InFolderToRename](const FLevelFolderItem& Folder)
					{
						bShouldRenameItem = Folder.Path == InFolderToRename;
					})
					.ActorFolder([&bShouldRenameItem, InFolderToRename](const FActorFolderItem& Folder)
					{
						bShouldRenameItem = Folder.Path == InFolderToRename;
					})
				);

				if (bShouldRenameItem)
				{
					TreeView->ClearSelection();
					TreeView->RequestNavigateToItem(Item);
					TreeView->SetItemSelection(Item, true);

					// widget for rows in the tree view may not ready yet. so request to rename is performed on the next tick.
					RegisterActiveTimer(0.0f, FWidgetActiveTimerDelegate::CreateSP(this, &STreeViewBase::RequestToRenameCreatedFolderItem, Item));

					break;
				}
			}
		}

		return EActiveTimerReturnType::Stop;
	}

	EActiveTimerReturnType STreeViewBase::RequestToRenameCreatedFolderItem(double CurrentTime, float DeltaTime, TSharedPtr<FBaseItem> Item)
	{
		if (Item.IsValid()) Item->OnRequestRename.ExecuteIfBound();
		return EActiveTimerReturnType::Stop;
	}

	void STreeViewBase::OpenColorPickerForSelectedFolders()
	{
		TArray<TSharedPtr<FBaseItem>> SelectedItems = GetSelectedItems();
		Temp_FolderColorsToChange.Empty(SelectedItems.Num());
		for (TSharedPtr<FBaseItem> SelectedItem : SelectedItems)
		{
			if (SelectedItem.IsValid())
			{
				if (FFolderProperties* FolderProperties = LevelStreamAssistantUtils::GetItemFolderProperties(*SelectedItem))
				{
					Temp_FolderColorsToChange.Add(&FolderProperties->Color);
				}
			}
		}

		FColorPickerArgs ColorPickerArgs;
		ColorPickerArgs.bIsModal = false;
		ColorPickerArgs.ParentWidget = SharedThis(this);
		ColorPickerArgs.bUseAlpha = false;
		ColorPickerArgs.bOnlyRefreshOnMouseUp = false;
		ColorPickerArgs.bOnlyRefreshOnOk = false;
		ColorPickerArgs.bExpandAdvancedSection = true;
		ColorPickerArgs.bOpenAsMenu = false;
		ColorPickerArgs.ColorArray = &Temp_FolderColorsToChange;
		ColorPickerArgs.InitialColorOverride = FLinearColor::White;

		ColorPickerArgs.OnInteractivePickBegin.BindSP(this, &STreeViewBase::OnChangeFolderColorInteractionBegin);
		ColorPickerArgs.OnInteractivePickEnd.BindSP(this, &STreeViewBase::OnChangeFolderColorInteractionEnd);
		ColorPickerArgs.OnColorPickerWindowClosed.BindSP(this, &STreeViewBase::OnChangeFolderColorWindowClosed);

		OpenColorPicker(ColorPickerArgs);
	}

	void STreeViewBase::RenameSelectedItem()
	{
		if (GetNumSelectedItems() == 1)
		{
			if (TSharedPtr<FBaseItem> SelectedItem = GetSelectedItems()[0])
			{
				SelectedItem->OnRequestRename.ExecuteIfBound();
			}
		}
	}

	void STreeViewBase::OnChangeFolderColorInteractionBegin()
	{
		if (ensure(GEditor)) GEditor->BeginTransaction(LOCTEXT("TransactionChangeFolderColor", "Change Folder Color"));

		if (UWorld* World = LevelStreamAssistantUtils::GetWorldFromEditor())
		{
			GetFolderManager().ModifyHierarchy(*World);
		}
	}

	void STreeViewBase::OnChangeFolderColorInteractionEnd()
	{
		if (ensure(GEditor)) GEditor->EndTransaction();
	}

	void STreeViewBase::OnChangeFolderColorWindowClosed(const TSharedRef<SWindow>& Window)
	{
		Temp_FolderColorsToChange.Empty();
	}
}

#undef LOCTEXT_NAMESPACE