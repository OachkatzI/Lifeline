// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Widgets/Views/STreeView.h>
#include "Widgets/DeclarativeSyntaxSupport.h"

class ITableRow;
class STableViewBase;

// Treeview that provides simpler approach for adding columns.
template<typename ItemType>
class STreeViewWithColumn : public STreeView<ItemType>
{
public:
	typedef typename STableRow<ItemType>::FArguments FTableRowArgs;

	DECLARE_DELEGATE_RetVal_TwoParams(TSharedRef<SWidget>, FOnGenerateWidget, TSharedRef<ITableRow>, ItemType);
	DECLARE_DELEGATE_RetVal_OneParam(FTableRowArgs, FOnGetTableRowArgs, ItemType);
	DECLARE_DELEGATE_RetVal_OneParam(TSharedPtr<SWidget>, FOnItemContextMenuOpening, ItemType);

	// represents column header arguments and a widget generator for that column.
	struct FColumn
	{
		SHeaderRow::FColumn::FArguments _HeaderArgs;

		FColumn& HeaderArgs(const SHeaderRow::FColumn::FArguments& InHeaderArgs)
		{
			_HeaderArgs = InHeaderArgs;
			return *this;
		}

		typedef FColumn WidgetArgsType;
		SLATE_EVENT(FOnGenerateWidget, OnGenerateWidget)
	};

	SLATE_BEGIN_ARGS(STreeViewWithColumn)
		: _TreeItemsSource(nullptr)
		, _ItemHeight(16)
		, _SelectionMode(ESelectionMode::Multi)
		, _ClearSelectionOnClick(true)
		, _EnableAnimatedScrolling(false)
		, _ScrollbarDragFocusCause(EFocusCause::Mouse)
		, _ConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible)
		, _AllowOverscroll(EAllowOverscroll::Yes)
		, _WheelScrollMultiplier(GetGlobalScrollAmount())
		, _HandleGamepadEvents(true)
		, _HandleDirectionalNavigation(true)
		, _AllowInvisibleItemSelection(false)
		, _HighlightParentNodesForSelection(false)
	{
	}
		SLATE_EVENT(FOnGetTableRowArgs, OnGetTableRowArgs)
		SLATE_EVENT(FOnItemContextMenuOpening, OnItemContextMenuOpening)
		SLATE_EVENT(FOnWidgetToBeRemoved, OnRowReleased)
		SLATE_EVENT(FOnTableViewScrolled, OnTreeViewScrolled)
		SLATE_EVENT(FOnItemScrolledIntoView, OnItemScrolledIntoView)
		SLATE_EVENT(FOnGetChildren, OnGetChildren)
		SLATE_EVENT(FOnSetExpansionRecursive, OnSetExpansionRecursive)
		SLATE_ARGUMENT(const TArray<ItemType>*, TreeItemsSource)
		SLATE_ATTRIBUTE(float, ItemHeight)
		SLATE_EVENT(FOnContextMenuOpening, OnContextMenuOpening)
		SLATE_EVENT(FOnMouseButtonClick, OnMouseButtonClick)
		SLATE_EVENT(FOnMouseButtonDoubleClick, OnMouseButtonDoubleClick)
		SLATE_EVENT(FOnSelectionChanged, OnSelectionChanged)
		SLATE_EVENT(FOnExpansionChanged, OnExpansionChanged)
		SLATE_EVENT(FIsSelectableOrNavigable, OnIsSelectableOrNavigable)
		SLATE_ATTRIBUTE(ESelectionMode::Type, SelectionMode)
		SLATE_ARGUMENT (bool, ClearSelectionOnClick)
		SLATE_ARGUMENT(TSharedPtr<SScrollBar>, ExternalScrollbar)
		SLATE_ARGUMENT(bool, EnableAnimatedScrolling)
		SLATE_ARGUMENT(TOptional<double>, FixedLineScrollOffset)
		SLATE_ARGUMENT(EFocusCause, ScrollbarDragFocusCause)
		SLATE_ARGUMENT(EConsumeMouseWheel, ConsumeMouseWheel);
		SLATE_ARGUMENT(EAllowOverscroll, AllowOverscroll);
		SLATE_ARGUMENT(float, WheelScrollMultiplier);
		/** Assign this to get more diagnostics from the list view. */
		SLATE_EVENT(FOnItemToString_Debug, OnItemToString_Debug)
		SLATE_EVENT(FOnTableViewBadState, OnEnteredBadState);
		SLATE_ARGUMENT(bool, HandleGamepadEvents);
		SLATE_ARGUMENT(bool, HandleDirectionalNavigation);
		SLATE_ARGUMENT(bool, AllowInvisibleItemSelection);
		SLATE_ARGUMENT(bool, HighlightParentNodesForSelection);

		// slots are used as columns.
		SLATE_SUPPORTS_SLOT(FColumn)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		OnGetTableRowArgs = InArgs._OnGetTableRowArgs;
		OnItemContextMenuOpening = InArgs._OnItemContextMenuOpening;

		TSharedPtr<SHeaderRow> HeaderRowWidget;

		STreeView<ItemType>::FArguments Args;
		Args.HeaderRow(SAssignNew(HeaderRowWidget, SHeaderRow))
			.OnGenerateRow(this, &STreeViewWithColumn<ItemType>::OnGenerateRow)
			.OnRowReleased(InArgs._OnRowReleased)
			.OnTreeViewScrolled(InArgs._OnTreeViewScrolled)
			.OnItemScrolledIntoView(InArgs._OnItemScrolledIntoView)
			.OnGetChildren(InArgs._OnGetChildren)
			.OnSetExpansionRecursive(InArgs._OnSetExpansionRecursive)
			.TreeItemsSource(InArgs._TreeItemsSource)
			.ItemHeight(InArgs._ItemHeight)
			.OnContextMenuOpening(InArgs._OnContextMenuOpening)
			.OnMouseButtonClick(InArgs._OnMouseButtonClick)
			.OnMouseButtonDoubleClick(InArgs._OnMouseButtonDoubleClick)
			.OnSelectionChanged(InArgs._OnSelectionChanged)
			.OnExpansionChanged(InArgs._OnExpansionChanged)
			.OnIsSelectableOrNavigable(InArgs._OnIsSelectableOrNavigable)
			.SelectionMode(InArgs._SelectionMode)
			.ClearSelectionOnClick(InArgs._ClearSelectionOnClick)
			.ExternalScrollbar(InArgs._ExternalScrollbar)
			.EnableAnimatedScrolling(InArgs._EnableAnimatedScrolling)
			.FixedLineScrollOffset(InArgs._FixedLineScrollOffset)
			.ScrollbarDragFocusCause(InArgs._ScrollbarDragFocusCause)
			.ConsumeMouseWheel(InArgs._ConsumeMouseWheel)
			.AllowOverscroll(InArgs._AllowOverscroll)
			.WheelScrollMultiplier(InArgs._WheelScrollMultiplier)
			.OnItemToString_Debug(InArgs._OnItemToString_Debug)
			.OnEnteredBadState(InArgs._OnEnteredBadState)
			.HandleGamepadEvents(InArgs._HandleGamepadEvents)
			.HandleDirectionalNavigation(InArgs._HandleDirectionalNavigation)
			.AllowInvisibleItemSelection(InArgs._AllowInvisibleItemSelection)
			.HighlightParentNodesForSelection(InArgs._HighlightParentNodesForSelection);

		// construct columns
		for (const STreeViewWithColumn<ItemType>::FColumn* Column : InArgs.Slots)
		{
			if (Column)
			{
				HeaderRowWidget->AddColumn(Column->_HeaderArgs);
				ColumnWidgetGenerators.Add(Column->_HeaderArgs._ColumnId, Column->_OnGenerateWidget);
				delete Column;
			}
		}

		STreeView<ItemType>::Construct(Args);
	}

	static STreeViewWithColumn<ItemType>::FColumn& Column()
	{
		return *(new STreeViewWithColumn<ItemType>::FColumn());
	}

private:

	TSharedRef<ITableRow> OnGenerateRow(ItemType InItem, const TSharedRef<STableViewBase>& InOwnerTable)
	{
		const FTableRowArgs& TableRowArgs = OnGetTableRowArgs.IsBound()
			? OnGetTableRowArgs.Execute(InItem) : FTableRowArgs();
		
		return SNew(SMultiColumnTableRowAuto<ItemType>, TableRowArgs, InOwnerTable)
			.OnGenerateWidgetForColumn(this, &STreeViewWithColumn::OnGenerateWidgetForColumn, InItem);
	}

	TSharedRef<SWidget> OnGenerateWidgetForColumn(TSharedRef<ITableRow> InTableRow, const FName& InColumn, ItemType InItem)
	{
		if (TDelegate<TSharedRef<SWidget>(TSharedRef<ITableRow>, ItemType)>* ColumnWidgetGenerator = ColumnWidgetGenerators.Find(InColumn))
		{
			if ((*ColumnWidgetGenerator).IsBound())
			{
				return (*ColumnWidgetGenerator).Execute(InTableRow, InItem);
			}
		}
		return SNullWidget::NullWidget;
	}

	virtual void Private_OnItemRightClicked(ItemType TheItem, const FPointerEvent& MouseEvent) override
	{
		const FVector2D& SummonLocation = MouseEvent.GetScreenSpacePosition();
		const bool bShouldOpenContextMenu = !IsRightClickScrolling();
		const bool bContextMenuOpeningBound = OnItemContextMenuOpening.IsBound();

		if (bShouldOpenContextMenu && bContextMenuOpeningBound)
		{
			// Get the context menu content. If NULL, don't open a menu.
			TSharedPtr<SWidget> MenuContent = OnItemContextMenuOpening.Execute(TheItem);

			if (MenuContent.IsValid())
			{
				//bShowSoftwareCursor = false;

				FWidgetPath WidgetPath = MouseEvent.GetEventPath() ? *MouseEvent.GetEventPath() : FWidgetPath();
				FSlateApplication::Get().PushMenu(AsShared(), WidgetPath, MenuContent.ToSharedRef(), SummonLocation, FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
			}
		}

		//AmountScrolledWhileRightMouseDown = 0;
	}
	
	FOnGetTableRowArgs OnGetTableRowArgs;
	FOnItemContextMenuOpening OnItemContextMenuOpening;
	TMap<FName, TDelegate<TSharedRef<SWidget>(TSharedRef<ITableRow>, ItemType)>> ColumnWidgetGenerators;
};

template<typename ItemType>
class SMultiColumnTableRowAuto : public SMultiColumnTableRow<ItemType>
{
public:
	DECLARE_DELEGATE_RetVal_TwoParams(TSharedRef<SWidget>, FOnGenerateWidgetForColumn, TSharedRef<ITableRow>, const FName&);

	SLATE_BEGIN_ARGS(SMultiColumnTableRowAuto) { }
		SLATE_EVENT(FOnGenerateWidgetForColumn, OnGenerateWidgetForColumn)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const FTableRowArgs& InParentArgs, TSharedRef<STableViewBase> InOwnerTableView)
	{
		OnGenerateWidgetForColumn = InArgs._OnGenerateWidgetForColumn;

		FSuperRowType::Construct(InParentArgs, InOwnerTableView);
	}

	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& InColumnName) override
	{
		if (OnGenerateWidgetForColumn.IsBound())
		{
			return OnGenerateWidgetForColumn.Execute(SharedThis(this), InColumnName);
		}
		return SNullWidget::NullWidget;
	}

private:
	FOnGenerateWidgetForColumn OnGenerateWidgetForColumn;
};
