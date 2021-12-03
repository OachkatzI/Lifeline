// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Widgets/SCompoundWidget.h>

template<typename ItemType> class STreeViewWithColumn;
class FReply;
struct FGeometry;
struct FPointerEvent;
class FDragDropEvent;
enum class EItemDropZone;
class FUICommandList;
class SWindow;
struct FColor;

namespace LevelStreamAssistant
{
	struct FBaseItem;
	class FFolderManager;

	class STreeViewBase : public SCompoundWidget
	{
	public:
		virtual void RequestTreeRefresh();
		virtual void RebuildTree();
		void ClearSelection();
		void SetItemSelection(const TSharedPtr<FBaseItem>& InItem, bool bSelected);
		void RequestScrollIntoView(const TSharedPtr<FBaseItem>& InItem);
		TArray<TSharedPtr<FBaseItem>> GetSelectedItems() const;
		int32 GetNumSelectedItems() const;

		void OnItemExpansionChanged(TSharedPtr<FBaseItem> Item, bool bExpanded) const;
		void UpdateItemsExpansion();

		void CreateFolderForSelectedItems();

	protected:
		FReply OnItemDragDetected(const FGeometry& Geometry, const FPointerEvent& PointerEvent);
		void OnItemDragLeave(FDragDropEvent const& DragDropEvent);
		TOptional<EItemDropZone> OnItemCanAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, TSharedPtr<FBaseItem> DropTarget);
		FReply OnItemAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, TSharedPtr<FBaseItem> DropTarget);

		virtual FFolderManager& GetFolderManager() const = 0;

		void OnGetRowChildren(TSharedPtr<FBaseItem> InItem, TArray<TSharedPtr<FBaseItem>>& OutChildren);

		virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
		
		void OpenColorPickerForSelectedFolders();

		void RenameSelectedItem();

		void CreateSubFolderForSelectedItem();

		TSharedPtr<STreeViewWithColumn<TSharedPtr<FBaseItem>>> TreeView;
		const TArray<TSharedPtr<FBaseItem>>* TreeItemsSource = nullptr;

		TSharedPtr<FUICommandList> CommandList;

	private:
		void MakeFolderRenameRequest(FName InFolderToRename);
		void HandlePendingRenameAfterCreateFolder();
		EActiveTimerReturnType RequestToRenameCreatedFolder(double CurrentTime, float DeltaTime, FName InFolderToRename);
		EActiveTimerReturnType RequestToRenameCreatedFolderItem(double CurrentTime, float DeltaTime, TSharedPtr<FBaseItem> Item);

		FName FolderToRename = NAME_None;
		bool bPendingRenameAfterCreateFolder = false;

		TArray<FColor*> Temp_FolderColorsToChange;
		void OnChangeFolderColorInteractionBegin();
		void OnChangeFolderColorInteractionEnd();
		void OnChangeFolderColorWindowClosed(const TSharedRef<SWindow>& Window);
	};
}