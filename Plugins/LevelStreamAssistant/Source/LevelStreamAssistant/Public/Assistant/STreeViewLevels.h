// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Assistant/STreeViewBase.h>
#include <Widgets/Views/STableRow.h>
#include "Widgets/DeclarativeSyntaxSupport.h"

class ITableRow;
enum class ECheckBoxState : uint8;
class ULevel;
class FUICommandInfo;
class FExtender;
namespace EExtensionHook { enum Position; }

namespace LevelStreamAssistant
{
	struct FBaseItem;
	class ILevelStreamingEditor;
	class FItemContextMenuModifier;

	class STreeViewLevels : public STreeViewBase
	{
	public:
		DECLARE_DELEGATE_OneParam(FOnItemHoverChanged, TSharedPtr<FBaseItem>);

		SLATE_BEGIN_ARGS(STreeViewLevels)
			: _TreeItemsSource(nullptr)
			, _CanToggleLevelVisbility(true)
		{ }

		SLATE_ARGUMENT(const TArray<TSharedPtr<FBaseItem>>*, TreeItemsSource)
		SLATE_ATTRIBUTE(bool, CanToggleLevelVisbility)

		SLATE_EVENT(FOnItemHoverChanged, OnItemHovered)
		SLATE_EVENT(FOnItemHoverChanged, OnItemUnhovered)

		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, TSharedRef<ILevelStreamingEditor> InLevelStreamingEditor);

		virtual void RequestTreeRefresh() override;
		virtual void RebuildTree() override;

		~STreeViewLevels();

	private:
		STableRow<TSharedPtr<FBaseItem>>::FArguments GetTableRowArgs(TSharedPtr<FBaseItem> InItem);

		TSharedRef<SWidget> OnGenerateVisibilityWidget(TSharedRef<ITableRow> Row, TSharedPtr<FBaseItem> Item);
		TSharedRef<SWidget> OnGenerateBindingModeWidget(TSharedRef<ITableRow> Row, TSharedPtr<FBaseItem> Item);
		TSharedRef<SWidget> OnGenerateNameWidget(TSharedRef<ITableRow> Row, TSharedPtr<FBaseItem> Item);

		FText GetBindingModeToolTip(TSharedPtr<FBaseItem> InItem);

		bool IsAnyActorSelected() const;

		void BindCommands();
		void PopulateContextMenuModifiers();
		void BuildContextMenuForLevelStreamingEditor(FExtender& MenuExtender) const;
		void ExtendMenuEntry(FExtender& Extender, const FName& Hook, EExtensionHook::Position Position, TSharedPtr<FUICommandInfo> CommandInfo);

		void OnItemHovered(TSharedPtr<FBaseItem> Item) const;
		void OnItemUnhovered(TSharedPtr<FBaseItem> Item) const;

		void OnItemRenamed(const FText& NewName, TSharedPtr<FBaseItem> InItem) const;

		void UpdateItemsHighlights();

		void OnSelectionChanged(TSharedPtr<FBaseItem> Entry, ESelectInfo::Type SelectInfo);
		
		TSharedPtr<SWidget> OnItemContextMenuOpening(TSharedPtr<FBaseItem> Item);
		bool ContextMenuAppliesToAllItems(const FItemContextMenuModifier& ContextMenuModifier, const TArray<TSharedPtr<FBaseItem>>& Items) const;
		
		TSharedRef<SWidget> GetSubMenu_ChangeStreamingMethod() const;

		FReply HandleDoubleClickOnItem(const FGeometry& InGeometry, const FPointerEvent& InEvent);

		void MakeCurrent();
		void MoveSelectedActorsToLevel();
		void MoveSelectedActorsToLevel(ULevel& InLevel);
		void ChangeStreamingMethodClass(UClass* StreamingClass);
		ECheckBoxState IsStreamingClassSetTo(UClass* StreamingClass);
		void FindLevelsInContentBrowser();
		void CreateFolder();
		void CreateSubFolder();
		void SetFoldersNewColor();
		void RenameItems();
		void DeleteItems();

		bool VerifyUserWantsToDeleteLevels(const TArray<TSharedPtr<FBaseItem>>& ItemsToDelete);

		virtual FFolderManager& GetFolderManager() const override;
		
		void OnSelectedLevelsChanged();

		bool bSelectionChangeInternal = false;
		bool bSelectionChangeExternal = false;

		FOnItemHoverChanged OnItemHoveredEvent;
		FOnItemHoverChanged OnItemUnhoveredEvent;

		TAttribute<bool> CanToggleLevelVisibility;

		TSharedPtr<ILevelStreamingEditor> LevelStreamingEditor;

		TArray<TSharedPtr<const FItemContextMenuModifier>> ContextMenuModifiers;
	};
}