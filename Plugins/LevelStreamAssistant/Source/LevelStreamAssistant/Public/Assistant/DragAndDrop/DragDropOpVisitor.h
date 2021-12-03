// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Assistant/TreeItemVisitors.h>

namespace LevelStreamAssistant
{
	struct FBaseItem;
	struct FLevelItem;
	struct FActorItem;
	struct FActorFolderItem;
	struct FLevelFolderItem;
	struct FActorRootItem;
	struct FLevelRootItem;

	// performs drag and drop on different items.
	struct FDragDropOpVisitor : public FItemVisitor
	{
		FDragDropOpVisitor(TSharedPtr<FBaseItem> InDropTarget) : DropTarget(InDropTarget) { }

		virtual ~FDragDropOpVisitor() override { }

		TSharedPtr<FBaseItem> DropTarget;
		
		virtual void Visit(const FActorItem& ItemToDrop) override;
		virtual void Visit(const FLevelItem& ItemToDrop) override;
		virtual void Visit(const FActorFolderItem& ItemToDrop) override;
		virtual void Visit(const FLevelFolderItem& ItemToDrop) override;
		virtual void Visit(const FActorRootItem& ItemToDrop) override { }
		virtual void Visit(const FLevelRootItem& ItemToDrop) override { }

	private:
		void DropActorOnFolder(const FActorItem& InDrop, const FActorFolderItem& InTarget);
		void DropLevelOnFolder(const FLevelItem& InDrop, const FLevelFolderItem& InTarget);
		void DropFolderOnFolder(const FActorFolderItem& InDrop, const FActorFolderItem& InTarget);
		void DropFolderOnFolder(const FLevelFolderItem& InDrop, const FLevelFolderItem& InTarget);
		void DropActorOnRoot(const FActorItem& InDrop, const FActorRootItem& InTarget);
		void DropLevelOnRoot(const FLevelItem& InDrop, const FLevelRootItem& InTarget);
		void DropFolderOnRoot(const FActorFolderItem& InDrop, const FActorRootItem& InTarget);
		void DropFolderOnRoot(const FLevelFolderItem& InDrop, const FLevelRootItem& InTarget);
	};
}