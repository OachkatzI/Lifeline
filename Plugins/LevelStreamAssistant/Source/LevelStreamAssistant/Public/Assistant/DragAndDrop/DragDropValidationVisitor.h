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

	// provides information on whether drag and drop can be performed on different items.
	struct FDragDropValidationVisitor : public FItemVisitor
	{
		FDragDropValidationVisitor(TSharedPtr<FBaseItem> InDropTarget);

		virtual ~FDragDropValidationVisitor() override { }

		TSharedPtr<FBaseItem> DropTarget;
		const FSlateBrush* Icon;
		FText Tooltip;
		bool bCanDrop;
		bool bShowToolTip;

		virtual void Visit(const FActorItem& ItemToDrop) override;
		virtual void Visit(const FLevelItem& ItemToDrop) override;
		virtual void Visit(const FActorFolderItem& ItemToDrop) override;
		virtual void Visit(const FLevelFolderItem& ItemToDrop) override;
		virtual void Visit(const FActorRootItem& ItemToDrop) override { }
		virtual void Visit(const FLevelRootItem& ItemToDrop) override { }

	private:
		void DropActorOnActor(const FActorItem& InDrop, const FActorItem& InTarget);
		void DropActorOnFolder(const FActorItem& InDrop, const FActorFolderItem& InTarget);
		void DropActorOnRoot(const FActorItem& InDrop, const FActorRootItem& InTarget);

		void DropLevelOnLevel(const FLevelItem& InDrop, const FLevelItem& InTarget);
		void DropLevelOnFolder(const FLevelItem& InDrop, const FLevelFolderItem& InTarget);
		void DropLevelOnRoot(const FLevelItem& InDrop, const FLevelRootItem& InTarget);

		void DropFolderOnActor(const FActorFolderItem& InDrop, const FActorItem& InTarget);
		void DropFolderOnLevel(const FLevelFolderItem& InDrop, const FLevelItem& InTarget);
		void DropFolderOnFolder(const FActorFolderItem& InDrop, const FActorFolderItem& InTarget);
		void DropFolderOnFolder(const FLevelFolderItem& InDrop, const FLevelFolderItem& InTarget);
		void DropFolderOnRoot(const FActorFolderItem& InDrop, const FActorRootItem& InTarget);
		void DropFolderOnRoot(const FLevelFolderItem& InDrop, const FLevelRootItem& InTarget);

		void DropFolderOnFolderImpl(const FString& ItemToDropPath, const FString& DropTargetPath);
		void DropFolderOnRootImpl(const FName& ItemToDropParentPath);

		static const FName Icon_Error;
		static const FName Icon_Ok;
	};
}
