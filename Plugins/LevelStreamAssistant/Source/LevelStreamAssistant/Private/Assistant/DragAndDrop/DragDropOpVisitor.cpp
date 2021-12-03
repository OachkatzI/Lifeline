// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Assistant/DragAndDrop/DragDropOpVisitor.h"
#include <Assistant/TreeItems.h>
#include <Utilities/LevelStreamAssistantItemUtils.h>
#include <ScopedTransaction.h>

#define LOCTEXT_NAMESPACE "LevelStreamAssistant.FDragDropOpVisitor"

namespace LevelStreamAssistant
{
	void FDragDropOpVisitor::Visit(const FActorItem& ItemToDrop)
	{
		if (!DropTarget.IsValid()) return;

		DropTarget->Accept(FFunctionalItemVisitor()
			.ActorFolder([&](const FActorFolderItem& FolderItem) { DropActorOnFolder(ItemToDrop, FolderItem); })
			.ActorRoot([&](const FActorRootItem& RootItem) { DropActorOnRoot(ItemToDrop, RootItem); })
		);
	}

	void FDragDropOpVisitor::Visit(const FLevelItem& ItemToDrop)
	{
		if (!DropTarget.IsValid()) return;

		DropTarget->Accept(FFunctionalItemVisitor()
			.LevelFolder([&](const FLevelFolderItem& FolderItem) { DropLevelOnFolder(ItemToDrop, FolderItem); })
			.LevelRoot([&](const FLevelRootItem& RootItem) { DropLevelOnRoot(ItemToDrop, RootItem); })
		);
	}

	void FDragDropOpVisitor::Visit(const FActorFolderItem& ItemToDrop)
	{
		if (!DropTarget.IsValid()) return;

		DropTarget->Accept(FFunctionalItemVisitor()
			.ActorFolder([&](const FActorFolderItem& FolderItem) { DropFolderOnFolder(ItemToDrop, FolderItem); })
			.ActorRoot([&](const FActorRootItem& RootItem) { DropFolderOnRoot(ItemToDrop, RootItem); })
		);
	}

	void FDragDropOpVisitor::Visit(const FLevelFolderItem& ItemToDrop)
	{
		if (!DropTarget.IsValid()) return;

		DropTarget->Accept(FFunctionalItemVisitor()
			.LevelFolder([&](const FLevelFolderItem& FolderItem) { DropFolderOnFolder(ItemToDrop, FolderItem); })
			.LevelRoot([&](const FLevelRootItem& RootItem) { DropFolderOnRoot(ItemToDrop, RootItem); })
		);
	}

	void FDragDropOpVisitor::DropActorOnFolder(const FActorItem& InDrop, const FActorFolderItem& InTarget)
	{
		FScopedTransaction Transaction(LOCTEXT("TransactionDropActorOnFolder", "Move Actor To Folder"));
		LevelStreamAssistantUtils::SetItemFolderPath(InDrop, InTarget.Path);
	}

	void FDragDropOpVisitor::DropLevelOnFolder(const FLevelItem& InDrop, const FLevelFolderItem& InTarget)
	{
		FScopedTransaction Transaction(LOCTEXT("TransactionDropLevelOnFolder", "Move Level To Folder"));
		LevelStreamAssistantUtils::SetItemFolderPath(InDrop, InTarget.Path);
	}

	void FDragDropOpVisitor::DropFolderOnFolder(const FActorFolderItem& InDrop, const FActorFolderItem& InTarget)
	{
		FScopedTransaction Transaction(LOCTEXT("TransactionDropActorFolderOnFolder", "Move Folder"));
		LevelStreamAssistantUtils::SetItemFolderPath(InDrop, InTarget.Path);
	}

	void FDragDropOpVisitor::DropFolderOnFolder(const FLevelFolderItem& InDrop, const FLevelFolderItem& InTarget)
	{
		FScopedTransaction Transaction(LOCTEXT("TransactionDropLevelFolderOnFolder", "Move Folder"));
		LevelStreamAssistantUtils::SetItemFolderPath(InDrop, InTarget.Path);
	}

	void FDragDropOpVisitor::DropActorOnRoot(const FActorItem& InDrop, const FActorRootItem& InTarget)
	{
		FScopedTransaction Transaction(LOCTEXT("TransactionDropActorOnRoot", "Move Actor To Root"));
		LevelStreamAssistantUtils::SetItemFolderPath(InDrop, NAME_None);
	}

	void FDragDropOpVisitor::DropLevelOnRoot(const FLevelItem& InDrop, const FLevelRootItem& InTarget)
	{
		FScopedTransaction Transaction(LOCTEXT("TransactionDropLevelOnRoot", "Move Level To Root"));
		LevelStreamAssistantUtils::SetItemFolderPath(InDrop, NAME_None);
	}

	void FDragDropOpVisitor::DropFolderOnRoot(const FActorFolderItem& InDrop, const FActorRootItem& InTarget)
	{
		FScopedTransaction Transaction(LOCTEXT("TransactionDropActorFolderOnRoot", "Move Folder To Root"));
		LevelStreamAssistantUtils::SetItemFolderPath(InDrop, NAME_None);
	}

	void FDragDropOpVisitor::DropFolderOnRoot(const FLevelFolderItem& InDrop, const FLevelRootItem& InTarget)
	{
		FScopedTransaction Transaction(LOCTEXT("TransactionDropLevelFolderOnRoot", "Move Folder To Root"));
		LevelStreamAssistantUtils::SetItemFolderPath(InDrop, NAME_None);
	}
}

#undef LOCTEXT_NAMESPACE
