// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Assistant/DragAndDrop/DragDropValidationVisitor.h"
#include <Assistant/TreeItems.h>
#include <EditorStyleSet.h>
#include <Utilities/LevelStreamAssistantItemUtils.h>

#define LOCTEXT_NAMESPACE "LevelStreamAssistant.FDragDropValidationVisitor"

namespace LevelStreamAssistant
{
	const FName FDragDropValidationVisitor::Icon_Error = TEXT("Graph.ConnectorFeedback.Error");
	const FName FDragDropValidationVisitor::Icon_Ok = TEXT("Graph.ConnectorFeedback.OK");

	FDragDropValidationVisitor::FDragDropValidationVisitor(TSharedPtr<FBaseItem> InDropTarget)
		: DropTarget(InDropTarget)
	{
		Tooltip = LOCTEXT("InvalidDragDropOperation", "Invalid drag and drop operation.");
		Icon = FEditorStyle::GetBrush(Icon_Error);
		bCanDrop = false;
		bShowToolTip = false;
	}

	void FDragDropValidationVisitor::Visit(const FActorItem& ItemToDrop)
	{
		if (!DropTarget.IsValid()) return;

		DropTarget->Accept(FFunctionalItemVisitor()
			.Actor([&](const FActorItem& DropTarget) { DropActorOnActor(ItemToDrop, DropTarget); })
			.ActorFolder([&](const FActorFolderItem& DropTarget) { DropActorOnFolder(ItemToDrop, DropTarget); })
			.ActorRoot([&](const FActorRootItem& DropTarget) { DropActorOnRoot(ItemToDrop, DropTarget); })
		);
	}

	void FDragDropValidationVisitor::Visit(const FLevelItem& ItemToDrop)
	{
		if (!DropTarget.IsValid()) return;

		DropTarget->Accept(FFunctionalItemVisitor()
			.Level([&](const FLevelItem& DropTarget) { DropLevelOnLevel(ItemToDrop, DropTarget); })
			.LevelFolder([&](const FLevelFolderItem& DropTarget) { DropLevelOnFolder(ItemToDrop, DropTarget); })
			.LevelRoot([&](const FLevelRootItem& DropTarget) { DropLevelOnRoot(ItemToDrop, DropTarget); })
		);
	}

	void FDragDropValidationVisitor::Visit(const FActorFolderItem& ItemToDrop)
	{
		if (!DropTarget.IsValid()) return;

		DropTarget->Accept(FFunctionalItemVisitor()
			.Actor([&](const FActorItem& DropTarget) { DropFolderOnActor(ItemToDrop, DropTarget); })
			.ActorFolder([&](const FActorFolderItem& DropTarget) { DropFolderOnFolder(ItemToDrop, DropTarget); })
			.ActorRoot([&](const FActorRootItem& DropTarget) { DropFolderOnRoot(ItemToDrop, DropTarget); })
		);
	}

	void FDragDropValidationVisitor::Visit(const FLevelFolderItem& ItemToDrop)
	{
		if (!DropTarget.IsValid()) return;

		DropTarget->Accept(FFunctionalItemVisitor()
			.Level([&](const FLevelItem& DropTarget) { DropFolderOnLevel(ItemToDrop, DropTarget); })
			.LevelFolder([&](const FLevelFolderItem& DropTarget) { DropFolderOnFolder(ItemToDrop, DropTarget); })
			.LevelRoot([&](const FLevelRootItem& DropTarget) { DropFolderOnRoot(ItemToDrop, DropTarget); })
		);
	}

	void FDragDropValidationVisitor::DropActorOnActor(const FActorItem& InDrop, const FActorItem& InTarget)
	{
		Tooltip = LOCTEXT("DropActorOnActor", "attaching actors is not supported.");
		Icon = FEditorStyle::GetBrush(Icon_Error);
		bCanDrop = false;
		bShowToolTip = true;
	}

	void FDragDropValidationVisitor::DropActorOnFolder(const FActorItem& InDrop, const FActorFolderItem& InTarget)
	{
		if (InDrop.Actor.IsValid())
		{
			FName ActorFolderPath = LevelStreamAssistantUtils::GetItemFolderPath(InDrop);
			if (ActorFolderPath == InTarget.Path)
			{
				Tooltip = LOCTEXT("DropActorOnFolderError", "actor is already in this folder.");
				Icon = FEditorStyle::GetBrush(Icon_Error);
				bCanDrop = false;
				bShowToolTip = true;
			}
			else
			{
				Tooltip = LOCTEXT("DropActorOnFolderOk", "move actor to this folder.");
				Icon = FEditorStyle::GetBrush(Icon_Ok);
				bCanDrop = true;
				bShowToolTip = true;
			}
		}
	}

	void FDragDropValidationVisitor::DropActorOnRoot(const FActorItem& InDrop, const FActorRootItem& InTarget)
	{
		if (InDrop.Actor.IsValid())
		{
			FName ActorFolderPath = LevelStreamAssistantUtils::GetItemFolderPath(InDrop);
			if (ActorFolderPath == NAME_None)
			{
				Tooltip = LOCTEXT("DropActorOnRootError", "actor is already in root.");
				Icon = FEditorStyle::GetBrush(Icon_Error);
				bCanDrop = false;
				bShowToolTip = true;
			}
			else
			{
				Tooltip = LOCTEXT("DropActorOnRootOk", "move actor to root.");
				Icon = FEditorStyle::GetBrush(Icon_Ok);
				bCanDrop = true;
				bShowToolTip = true;
			}
		}
	}

	void FDragDropValidationVisitor::DropLevelOnLevel(const FLevelItem& InDrop, const FLevelItem& InTarget)
	{
		Tooltip = LOCTEXT("DropLevelOnLevel", "attaching levels is not supported.");
		Icon = FEditorStyle::GetBrush(Icon_Error);
		bCanDrop = false;
		bShowToolTip = true;
	}

	void FDragDropValidationVisitor::DropLevelOnFolder(const FLevelItem& InDrop, const FLevelFolderItem& InTarget)
	{
		if (InDrop.Level.IsValid())
		{
			FName LevelFolderPath = LevelStreamAssistantUtils::GetItemFolderPath(InDrop);
			if (LevelFolderPath == InTarget.Path)
			{
				Tooltip = LOCTEXT("DropLevelOnFolderError", "level is already in this folder.");
				Icon = FEditorStyle::GetBrush(Icon_Error);
				bCanDrop = false;
				bShowToolTip = true;
			}
			else
			{
				Tooltip = LOCTEXT("DropLevelOnFolderOk", "move level to this folder.");
				Icon = FEditorStyle::GetBrush(Icon_Ok);
				bCanDrop = true;
				bShowToolTip = true;
			}
		}
	}

	void FDragDropValidationVisitor::DropLevelOnRoot(const FLevelItem& InDrop, const FLevelRootItem& InTarget)
	{
		if (InDrop.Level.IsValid())
		{
			FName LevelFolderPath = LevelStreamAssistantUtils::GetItemFolderPath(InDrop);
			if (LevelFolderPath == NAME_None)
			{
				Tooltip = LOCTEXT("DropLevelOnRootError", "level is already in root.");
				Icon = FEditorStyle::GetBrush(Icon_Error);
				bCanDrop = false;
				bShowToolTip = true;
			}
			else
			{
				Tooltip = LOCTEXT("DropLevelOnRootOk", "move level to root.");
				Icon = FEditorStyle::GetBrush(Icon_Ok);
				bCanDrop = true;
				bShowToolTip = true;
			}
		}
	}

	void FDragDropValidationVisitor::DropFolderOnActor(const FActorFolderItem& InDrop, const FActorItem& InTarget)
	{
		Tooltip = LOCTEXT("DropFolderOnActor", "can not drop folder on actor.");
		Icon = FEditorStyle::GetBrush(Icon_Error);
		bCanDrop = false;
		bShowToolTip = true;
	}

	void FDragDropValidationVisitor::DropFolderOnLevel(const FLevelFolderItem& InDrop, const FLevelItem& InTarget)
	{
		Tooltip = LOCTEXT("DropFolderOnLevel", "can not drop folder on level.");
		Icon = FEditorStyle::GetBrush(Icon_Error);
		bCanDrop = false;
		bShowToolTip = true;
	}

	void FDragDropValidationVisitor::DropFolderOnFolder(const FActorFolderItem& InDrop, const FActorFolderItem& InTarget)
	{
		FString ItemToDropPath = InDrop.Path.ToString();
		FString DropTargetPath = InTarget.Path.ToString();
		DropFolderOnFolderImpl(ItemToDropPath, DropTargetPath);
	}
	
	void FDragDropValidationVisitor::DropFolderOnFolder(const FLevelFolderItem& InDrop, const FLevelFolderItem& InTarget)
	{
		FString ItemToDropPath = InDrop.Path.ToString();
		FString DropTargetPath = InTarget.Path.ToString();
		DropFolderOnFolderImpl(ItemToDropPath, DropTargetPath);
	}

	void FDragDropValidationVisitor::DropFolderOnRoot(const FActorFolderItem& InDrop, const FActorRootItem& InTarget)
	{
		FName ActorFolderPath = LevelStreamAssistantUtils::GetItemFolderPath(InDrop);
		DropFolderOnRootImpl(ActorFolderPath);
	}

	void FDragDropValidationVisitor::DropFolderOnRoot(const FLevelFolderItem& InDrop, const FLevelRootItem& InTarget)
	{
		FName LevelFolderPath = LevelStreamAssistantUtils::GetItemFolderPath(InDrop);
		DropFolderOnRootImpl(LevelFolderPath);
	}

	void FDragDropValidationVisitor::DropFolderOnFolderImpl(const FString& ItemToDropPath, const FString& DropTargetPath)
	{
		if (FPaths::IsSamePath(DropTargetPath, ItemToDropPath))
		{
			Tooltip = LOCTEXT("DropFolderOnSelf", "can not drop folder on itself.");
			Icon = FEditorStyle::GetBrush(Icon_Error);
			bCanDrop = false;
			bShowToolTip = true;
		}
		// if DropTargetPath is under ItemToDropPath
		else if (FPaths::IsUnderDirectory(DropTargetPath, ItemToDropPath))
		{
			Tooltip = LOCTEXT("DropFolderOnDescendant", "can not drop folder on it's descendant.");
			Icon = FEditorStyle::GetBrush(Icon_Error);
			bCanDrop = false;
			bShowToolTip = true;
		}
		else if (FPaths::IsSamePath(DropTargetPath, FPaths::GetPath(ItemToDropPath)))
		{
			Tooltip = LOCTEXT("DropFolderOnSameParent", "folder is already in this folder.");
			Icon = FEditorStyle::GetBrush(Icon_Error);
			bCanDrop = false;
			bShowToolTip = true;
		}
		else
		{
			Tooltip = LOCTEXT("DropFolderOnFolderOk", "move folder to this folder.");
			Icon = FEditorStyle::GetBrush(Icon_Ok);
			bCanDrop = true;
			bShowToolTip = true;
		}
	}

	void FDragDropValidationVisitor::DropFolderOnRootImpl(const FName& ItemToDropParentPath)
	{
		if (ItemToDropParentPath == NAME_None)
		{
			Tooltip = LOCTEXT("DropFolderOnRootError", "folder is already in root.");
			Icon = FEditorStyle::GetBrush(Icon_Error);
			bCanDrop = false;
			bShowToolTip = true;
		}
		else
		{
			Tooltip = LOCTEXT("DropFolderOnRootOk", "move folder to root.");
			Icon = FEditorStyle::GetBrush(Icon_Ok);
			bCanDrop = true;
			bShowToolTip = true;
		}
	}
}

#undef LOCTEXT_NAMESPACE
