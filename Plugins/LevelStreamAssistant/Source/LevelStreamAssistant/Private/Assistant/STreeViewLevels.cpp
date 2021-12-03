// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Assistant/STreeViewLevels.h" 
#include <Assistant/TreeItems.h>
#include <LevelStreamAssistantStyle.h>
#include <Assistant/ItemWidgets/SItemLabel.h>
#include <Assistant/ItemWidgets/SItemIcon.h>
#include <Assistant/ItemWidgets/SLevelBinding.h>
#include <Assistant/ItemWidgets/SToggleVisibility.h>
#include <Utilities/LevelStreamAssistantItemUtils.h>
#include <Widgets/STreeViewWithColumn.h>
#include <Widgets/SHoverBox.h>
#include <Assistant/LevelStreamingEditor.h>
#include <ScopedTransaction.h>
#include <Iterators/ItemIterators.h>
#include <LevelStreamBindingMode.h>
#include <Utilities/LevelStreamingBindingUtils.h>
#include <Framework/MultiBox/MultiBoxBuilder.h>
#include <Assistant/TreeItemVisitors.h>
#include <EditorLevelUtils.h>
#include <Editor.h>
#include <Engine/Selection.h>
#include <Engine/LevelStreaming.h>
#include <Engine/LevelStreamingDynamic.h>
#include <Engine/LevelStreamingAlwaysLoaded.h>
#include <Framework/Commands/UICommandList.h>
#include <Framework/Commands/GenericCommands.h>
#include <LevelStreamAssistantCommands.h>
#include <Assistant/Folders/FolderManager.h>
#include <Assistant/ItemContextMenuModifier.h>
#include <Assistant/LevelStreamAssistantHierarchyView.h>
#include <LevelStreamingActorEditorBase.h>
#include <Dialogs/Dialogs.h>

#define LOCTEXT_NAMESPACE "LevelStreamAssistant.STreeViewLevels"

namespace LevelStreamAssistant
{
	void STreeViewLevels::Construct(const FArguments& InArgs, TSharedRef<ILevelStreamingEditor> InLevelStreamingEditor)
	{
		LevelStreamingEditor = InLevelStreamingEditor;

		LevelStreamingEditor->OnSelectedLevelsChanged().AddSP(this, &STreeViewLevels::OnSelectedLevelsChanged);

		CanToggleLevelVisibility = InArgs._CanToggleLevelVisbility;

		OnItemHoveredEvent = InArgs._OnItemHovered;
		OnItemUnhoveredEvent = InArgs._OnItemUnhovered;

		TreeItemsSource = InArgs._TreeItemsSource;
		check(TreeItemsSource);

		BindCommands();
		PopulateContextMenuModifiers();

		ChildSlot
		[
			SAssignNew(TreeView, STreeViewWithColumn<TSharedPtr<FBaseItem>>)
			.TreeItemsSource(InArgs._TreeItemsSource)
			.SelectionMode(ESelectionMode::Multi)
			.AllowInvisibleItemSelection(true) // allows to keep items selection when they are filtered out.
			.OnGetChildren(this, &STreeViewLevels::OnGetRowChildren)
			.OnGetTableRowArgs(this, &STreeViewLevels::GetTableRowArgs)
			.OnExpansionChanged(this, &STreeViewLevels::OnItemExpansionChanged)
			.OnSelectionChanged(this, &STreeViewLevels::OnSelectionChanged)
			.OnItemContextMenuOpening(this, &STreeViewLevels::OnItemContextMenuOpening)
			+ STreeViewWithColumn<TSharedPtr<FBaseItem>>::Column()
			.OnGenerateWidget(this, &STreeViewLevels::OnGenerateVisibilityWidget)
			.HeaderArgs(
				SHeaderRow::Column("Visibility")
				.DefaultLabel(FText::GetEmpty())
				.VAlignCell(VAlign_Center)
				.HAlignCell(HAlign_Center)
				.FixedWidth(21.f)
			)
			+ STreeViewWithColumn<TSharedPtr<FBaseItem>>::Column()
			.OnGenerateWidget(this, &STreeViewLevels::OnGenerateBindingModeWidget)
			.HeaderArgs(
				SHeaderRow::Column("BindingMode")
				.DefaultLabel(FText::GetEmpty())
				.VAlignCell(VAlign_Center)
				.HAlignCell(HAlign_Center)
				.FixedWidth(21.f)
			)
			+ STreeViewWithColumn<TSharedPtr<FBaseItem>>::Column()
			.OnGenerateWidget(this, &STreeViewLevels::OnGenerateNameWidget)
			.HeaderArgs(
				SHeaderRow::Column("Name")
				.DefaultLabel(LOCTEXT("NameHeader", "Name"))
				.FillWidth(1)
				[
					SNew(SBox)
					.MinDesiredHeight(22)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock).Text(LOCTEXT("LabelStreamingLevels", "Streaming Levels"))
					]
				]
			)
		];

		UpdateItemsExpansion();
		UpdateItemsHighlights();
	}

	STreeViewLevels::~STreeViewLevels()
	{
		if (LevelStreamingEditor.IsValid())
		{
			LevelStreamingEditor->OnSelectedLevelsChanged().RemoveAll(this);
		}
	}

	TSharedRef<SWidget> STreeViewLevels::OnGenerateVisibilityWidget(TSharedRef<ITableRow> Row, TSharedPtr<FBaseItem> Item)
	{
		return SNew(SToggleVisibility, Item.ToSharedRef(), TreeItemsSource)
			.IsEnabled(CanToggleLevelVisibility);
	}
	
	TSharedRef<SWidget> STreeViewLevels::OnGenerateBindingModeWidget(TSharedRef<ITableRow> Row, TSharedPtr<FBaseItem> Item)
	{
		return SNew(SLevelBinding, Item.ToSharedRef(), LevelStreamingEditor.ToSharedRef())
			.ToolTipText(GetBindingModeToolTip(Item))
			.OnHovered(this, &STreeViewLevels::OnItemHovered, Item)
			.OnUnhovered(this, &STreeViewLevels::OnItemUnhovered, Item);
	}

	TSharedRef<SWidget> STreeViewLevels::OnGenerateNameWidget(TSharedRef<ITableRow> Row, TSharedPtr<FBaseItem> Item)
	{
		TSharedRef<SWidget> NameWidget = SNew(SHoverBox)
		.OnHovered(this, &STreeViewLevels::OnItemHovered, Item)
		.OnUnhovered(this, &STreeViewLevels::OnItemUnhovered, Item)
		[
			SNew(SBox)
			.MinDesiredHeight(20.f)
			.VAlign(VAlign_Center)
			[ 
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SExpanderArrow, Row)
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SItemIcon, Row, Item.ToSharedRef())
				]
				+SHorizontalBox::Slot()
				[
					SNew(SItemLabel, Row, Item.ToSharedRef())
					.OnLabelEdited(this, &STreeViewLevels::OnItemRenamed, Item)
				]
			]
		];

		NameWidget->SetOnMouseDoubleClick(FPointerEventHandler::CreateSP(this, &STreeViewLevels::HandleDoubleClickOnItem));

		return NameWidget;
	}

	STableRow<TSharedPtr<FBaseItem>>::FArguments STreeViewLevels::GetTableRowArgs(TSharedPtr<FBaseItem> InItem)
	{
		return STableRow<TSharedPtr<FBaseItem>>::FArguments()
			.Style(&FLevelStreamAssistantStyle::Get().GetWidgetStyle<FTableRowStyle>("LSA.ListViewLevel.Row"))
			.OnDragDetected(this, &STreeViewLevels::OnItemDragDetected)
			.OnDragLeave(this, &STreeViewLevels::OnItemDragLeave)
			.OnCanAcceptDrop(this, &STreeViewLevels::OnItemCanAcceptDrop)
			.OnAcceptDrop(this, &STreeViewLevels::OnItemAcceptDrop);
	}

	FText STreeViewLevels::GetBindingModeToolTip(TSharedPtr<FBaseItem> InItem)
	{
		FText ToolTipText;

		TSharedPtr<FLevelStreamBindingMode> BindingMode;
		switch (GetItemBindingModeToSelectedActors(LevelStreamingEditor, InItem, BindingMode))
		{
		case EBindingModeResult::Success:
			ToolTipText = BindingMode->GetToolTip();
			break;

		case EBindingModeResult::MultipleBindings:
			ToolTipText = LOCTEXT("TooltipMultipleSelections", "Multiple bindings.");
			break;

		case EBindingModeResult::NoBinding:
		default:
			ToolTipText = FText::GetEmpty();
			break;
		}

		return ToolTipText;
	}

	TSharedPtr<SWidget> STreeViewLevels::OnItemContextMenuOpening(TSharedPtr<FBaseItem> Item)
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender);
		FMenuBuilder MenuBuilder(true, CommandList, MenuExtender);

		MenuBuilder.BeginSection("Level", LOCTEXT("LabelLevelSection", "Level"));

		TArray<TSharedPtr<FBaseItem>> SelectedItems = GetSelectedItems();
		int32 NumSelectedItems = GetNumSelectedItems();
		if (NumSelectedItems > 0)
		{
			for (TSharedPtr<const FItemContextMenuModifier> ContextMenuModifier : ContextMenuModifiers)
			{
				if (!ensure(ContextMenuModifier.IsValid())) continue;

				bool bAppliesToAllItems = ContextMenuAppliesToAllItems(*ContextMenuModifier, SelectedItems);
				bool bSelectionIsSupported = NumSelectedItems == 1 || ContextMenuModifier->DoesSupportMultipleSelection();

				if (bAppliesToAllItems && bSelectionIsSupported)
				{
					ContextMenuModifier->ModifyMenu(*MenuExtender);
				}
			}
		}

		MenuBuilder.EndSection();

		return MenuBuilder.MakeWidget();
	}

	bool STreeViewLevels::ContextMenuAppliesToAllItems(const FItemContextMenuModifier& ContextMenuModifier, const TArray<TSharedPtr<FBaseItem>>& Items) const
	{
		for (TSharedPtr<FBaseItem> Item : Items)
		{
			if (!Item.IsValid()) continue;

			if (!ContextMenuModifier.DoesApplyToItem(*Item))
			{
				return false;
			}
		}
		return true;
	}

	TSharedRef<SWidget> STreeViewLevels::GetSubMenu_ChangeStreamingMethod() const
	{
		const FLevelStreamAssistantCommands& Commands = FLevelStreamAssistantCommands::GetSafe();
		FMenuBuilder MenuBuilder(true, CommandList);
		{
			MenuBuilder.AddMenuEntry(Commands.ChangeStreamingMethodToBlueprint);
			MenuBuilder.AddMenuEntry(Commands.ChangeStreamingMethodToAlwaysLoaded);
		}
		return MenuBuilder.MakeWidget();
	}

	FReply STreeViewLevels::HandleDoubleClickOnItem(const FGeometry& InGeometry, const FPointerEvent& InEvent)
	{
		// when a level item is double clicked, it becomes the current level.
		MakeCurrent();

		return FReply::Handled();
	}

	void STreeViewLevels::RequestTreeRefresh()
	{
		STreeViewBase::RequestTreeRefresh();

		UpdateItemsHighlights();
	}

	void STreeViewLevels::RebuildTree()
	{
		STreeViewBase::RebuildTree();

		UpdateItemsHighlights();
	}

	bool STreeViewLevels::IsAnyActorSelected() const
	{
		return LevelStreamingEditor->GetSelectedLevelStreamingActors().Num() > 0;
	}

	void STreeViewLevels::OnItemHovered(TSharedPtr<FBaseItem> Item) const
	{
		OnItemHoveredEvent.ExecuteIfBound(Item);
	}

	void STreeViewLevels::OnItemUnhovered(TSharedPtr<FBaseItem> Item) const
	{
		OnItemUnhoveredEvent.ExecuteIfBound(Item);
	}

	void STreeViewLevels::OnItemRenamed(const FText& NewName, TSharedPtr<FBaseItem> InItem) const
	{
		if (InItem.IsValid())
		{
			FScopedTransaction Transaction(LOCTEXT("TransactionRenameItem", "Rename Item"));
			LevelStreamAssistantUtils::RenameItem(*InItem, *NewName.ToString());
		}
	}

	void STreeViewLevels::UpdateItemsHighlights()
	{
		for (TSharedPtr<FBaseItem> Item : FItemHierarchyRange(*TreeItemsSource))
		{
			if (Item.IsValid())
			{
				TSharedPtr<FLevelStreamBindingMode> BindingMode;
				EBindingModeResult Result = GetItemBindingModeToSelectedActors(LevelStreamingEditor, Item, BindingMode);
				bool bLevelItemIsBoundToAnySelectedActor = Result != EBindingModeResult::NoBinding;
				TreeView->SetItemHighlighted(Item, bLevelItemIsBoundToAnySelectedActor);
			}
		}
	}

	void STreeViewLevels::OnSelectionChanged(TSharedPtr<FBaseItem> Entry, ESelectInfo::Type SelectInfo)
	{
		if (TreeView.IsValid() && !bSelectionChangeExternal)
		{
			LevelStreamingEditor->ClearSelectedStreamingLevels();

			FFunctionalItemVisitor AddLevel =
				FFunctionalItemVisitor().Level([&LevelStreamingEditor = LevelStreamingEditor](const FLevelItem& LevelItem)
					{
						LevelStreamingEditor->SelectStreamingLevel(LevelItem.Level);
					});

			for (TSharedPtr<FBaseItem> Item : TreeView->GetSelectedItems())
			{
				if (Item.IsValid())
				{
					Item->Accept(AddLevel);
				}
			}

			bSelectionChangeInternal = true;
			LevelStreamingEditor->NotifySelectedLevelsChanged();
			bSelectionChangeInternal = false;
		}
	}

	void STreeViewLevels::OnSelectedLevelsChanged()
	{
		if (TreeItemsSource && TreeView.IsValid() && !bSelectionChangeInternal)
		{
			TSet<TWeakObjectPtr<ULevelStreaming>> SelectedLevels(LevelStreamingEditor->GetSelectedStreamingLevels());

			TArray<TSharedPtr<FBaseItem>> ItemsToSelect;
			ItemsToSelect.Reserve(SelectedLevels.Num());

			FFunctionalItemVisitor SelectItem =
				FFunctionalItemVisitor().Level([&SelectedLevels, &ItemsToSelect](const FLevelItem& LevelItem)
					{
						if(SelectedLevels.Contains(LevelItem.Level))
						{
							ItemsToSelect.Add(ConstCastSharedRef<FBaseItem>(LevelItem.AsShared()));
						}
					});

			for (TSharedPtr<FBaseItem> Item : FItemHierarchyRange(*TreeItemsSource))
			{
				if (Item.IsValid())
				{
					Item->Accept(SelectItem);
				}
			}

			bSelectionChangeExternal = true;
			TreeView->ClearSelection();
			TreeView->SetItemSelection(ItemsToSelect, true, ESelectInfo::Direct);
			bSelectionChangeExternal = false;
		}
	}

	void STreeViewLevels::BindCommands()
	{
		const FLevelStreamAssistantCommands& Commands = FLevelStreamAssistantCommands::GetSafe();
		const FGenericCommands& GenericCommands = FGenericCommands::Get();

		CommandList = MakeShareable(new FUICommandList);

		CommandList->MapAction(
			Commands.MakeCurrent,
			FExecuteAction::CreateSP(this, &STreeViewLevels::MakeCurrent)
		);

		CommandList->MapAction(
			Commands.MoveSelectedActorsToLevel,
			FExecuteAction::CreateSP(this, &STreeViewLevels::MoveSelectedActorsToLevel)
		);

		CommandList->MapAction(
			Commands.ChangeStreamingMethodToBlueprint,
			FExecuteAction::CreateSP(this, &STreeViewLevels::ChangeStreamingMethodClass, ULevelStreamingDynamic::StaticClass()),
			FCanExecuteAction(),
			FGetActionCheckState::CreateSP(this, &STreeViewLevels::IsStreamingClassSetTo, ULevelStreamingDynamic::StaticClass())
		);

		CommandList->MapAction(
			Commands.ChangeStreamingMethodToAlwaysLoaded,
			FExecuteAction::CreateSP(this, &STreeViewLevels::ChangeStreamingMethodClass, ULevelStreamingAlwaysLoaded::StaticClass()),
			FCanExecuteAction(),
			FGetActionCheckState::CreateSP(this, &STreeViewLevels::IsStreamingClassSetTo, ULevelStreamingAlwaysLoaded::StaticClass())
		);

		CommandList->MapAction(
			Commands.FindLevelsInContentBrowser,
			FExecuteAction::CreateSP(this, &STreeViewLevels::FindLevelsInContentBrowser)
		);

		CommandList->MapAction(
			Commands.SetFoldersNewColor,
			FExecuteAction::CreateSP(this, &STreeViewLevels::SetFoldersNewColor)
		);

		CommandList->MapAction(
			GenericCommands.Rename,
			FExecuteAction::CreateSP(this, &STreeViewLevels::RenameItems)
		);

		CommandList->MapAction(
			GenericCommands.Delete,
			FExecuteAction::CreateSP(this, &STreeViewLevels::DeleteItems)
		);

		CommandList->MapAction(
			Commands.CreateFolder,
			FExecuteAction::CreateSP(this, &STreeViewLevels::CreateFolder)
		);

		CommandList->MapAction(
			Commands.CreateSubFolder,
			FExecuteAction::CreateSP(this, &STreeViewLevels::CreateSubFolder)
		);
	}

	void STreeViewLevels::PopulateContextMenuModifiers()
	{
		const FLevelStreamAssistantCommands& Commands = FLevelStreamAssistantCommands::GetSafe();
		const FGenericCommands& GenericCommands = FGenericCommands::Get();

		ContextMenuModifiers.Empty();

		TFunction<void(const FItemContextMenuModifier&)> AddContextMenuModifier
			= [&Modifiers = ContextMenuModifiers](const FItemContextMenuModifier& Modifier)
		{
			Modifiers.Add(MakeShareable(&Modifier));
		};

		AddContextMenuModifier(FItemContextMenuModifier::New()
			.OnModifyMenu([this, &Commands](FExtender& Extender) { ExtendMenuEntry(Extender, "Level", EExtensionHook::After, Commands.MakeCurrent); })
			.AppliesToLevel()
			.AppliesToLevelRoot()
		);

		AddContextMenuModifier(FItemContextMenuModifier::New()
			.OnModifyMenu([this, &Commands](FExtender& Extender) { ExtendMenuEntry(Extender, "Level", EExtensionHook::After, Commands.MoveSelectedActorsToLevel); })
			.AppliesToLevel()
			.AppliesToLevelRoot()
		);

		AddContextMenuModifier(FItemContextMenuModifier::New()
			.OnModifyMenu([this, &GenericCommands](FExtender& Extender) { ExtendMenuEntry(Extender, "Level", EExtensionHook::After, GenericCommands.Rename); })
			.AppliesToLevelFolder()
		);

		AddContextMenuModifier(FItemContextMenuModifier::New()
			.OnModifyMenu([this, &GenericCommands](FExtender& Extender) { ExtendMenuEntry(Extender, "Level", EExtensionHook::After, GenericCommands.Delete); })
			.SupportsMultipleSelection()
			.AppliesToLevel()
			.AppliesToLevelFolder()
		);

		AddContextMenuModifier(FItemContextMenuModifier::New()
			.OnModifyMenu([this, &Commands](FExtender& Extender) { ExtendMenuEntry(Extender, "Level", EExtensionHook::After, Commands.SetFoldersNewColor); })
			.SupportsMultipleSelection()
			.AppliesToLevelFolder()
		);

		AddContextMenuModifier(FItemContextMenuModifier::New()
			.OnModifyMenu([this](FExtender& Extender) {
				Extender.AddMenuExtension("Level", EExtensionHook::After, CommandList,
					FMenuExtensionDelegate::CreateLambda([this](FMenuBuilder& MenuBuilder)
					{
						MenuBuilder.AddWrapperSubMenu(
							LOCTEXT("LabelChangeStreamingMethod", "Change Streaming Method"),
							LOCTEXT("TooltipChangeStreamingMethod", "Change streaming method for selected levels."),
							FOnGetContent::CreateSP(this, &STreeViewLevels::GetSubMenu_ChangeStreamingMethod),
							FSlateIcon()
						);
					})
				);
			})
			.SupportsMultipleSelection()
			.AppliesToLevel()
			.AppliesToLevelFolder()
		);

		AddContextMenuModifier(FItemContextMenuModifier::New()
			.OnModifyMenu([this, &Commands](FExtender& Extender) { ExtendMenuEntry(Extender, "Level", EExtensionHook::After, Commands.FindLevelsInContentBrowser); })
			.SupportsMultipleSelection()
			.AppliesToLevel()
			.AppliesToLevelFolder()
			.AppliesToLevelRoot()
		);

		AddContextMenuModifier(FItemContextMenuModifier::New()
			.OnModifyMenu([this, &Commands](FExtender& Extender) { ExtendMenuEntry(Extender, "Level", EExtensionHook::After, Commands.CreateFolder); })
			.SupportsMultipleSelection()
			.AppliesToLevel()
			.AppliesToLevelFolder()
		);

		AddContextMenuModifier(FItemContextMenuModifier::New()
			.OnModifyMenu([this, &Commands](FExtender& Extender) { ExtendMenuEntry(Extender, "Level", EExtensionHook::After, Commands.CreateSubFolder); })
			.AppliesToLevelFolder()
			.AppliesToLevelRoot()
		);

		AddContextMenuModifier(FItemContextMenuModifier::New()
			.OnModifyMenu([this](FExtender& Extender) { BuildContextMenuForLevelStreamingEditor(Extender); })
			.SupportsMultipleSelection()
			.AppliesToLevel()
		);
	}

	void STreeViewLevels::ExtendMenuEntry(FExtender& Extender, const FName& Hook, EExtensionHook::Position Position, TSharedPtr<FUICommandInfo> CommandInfo)
	{
		Extender.AddMenuExtension(Hook, Position, CommandList,
			FMenuExtensionDelegate::CreateLambda([CommandInfo](FMenuBuilder& MenuBuilder) { MenuBuilder.AddMenuEntry(CommandInfo); }));
	};

	void STreeViewLevels::BuildContextMenuForLevelStreamingEditor(FExtender& MenuExtender) const
	{
		TArray<TSharedPtr<FBaseItem>> SelectedItems = TreeView->GetSelectedItems();
	
		TArray<ULevelStreaming*> SelectedLevels;
		SelectedLevels.Reserve(SelectedItems.Num());
		
		for (TSharedPtr<FBaseItem> SelectedItem : SelectedItems)
		{
			if (SelectedItem.IsValid())
			{
				SelectedItem->Accept(FFunctionalItemVisitor()
					.Level([&SelectedLevels](const FLevelItem& LevelItem)
					{
						if (ULevelStreaming* SelectedLevel = LevelItem.Level.Get())
						{
							SelectedLevels.Add(SelectedLevel);
						}
					})
				);
			}
		}

		TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView = MakeShareable(new FLevelStreamAssistantHierarchyView(LevelStreamingEditor));
		LevelStreamingEditor->GetLevelStreamingActorEditor().ExtendLevelsContextMenu(MenuExtender, HierarchyView);
	}

	void STreeViewLevels::MakeCurrent()
	{
		if (GetNumSelectedItems() == 1)
		{
			if (TSharedPtr<FBaseItem> SelectedItem = GetSelectedItems()[0])
			{
				SelectedItem->Accept(FFunctionalItemVisitor()
					.Level([](const FLevelItem& LevelItem)
					{
						if(LevelItem.Level.IsValid())
						{
							EditorLevelUtils::MakeLevelCurrent(LevelItem.Level.Get());
						}
					})
					.LevelRoot([](const FLevelRootItem& RootItem)
					{
						if (RootItem.PersistentLevel.IsValid())
						{
							EditorLevelUtils::MakeLevelCurrent(RootItem.PersistentLevel.Get());
						}
					})
				);
			}
		}
	}

	void STreeViewLevels::MoveSelectedActorsToLevel()
	{
		if (GetNumSelectedItems() == 1)
		{
			if (TSharedPtr<FBaseItem> SelectedItem = GetSelectedItems()[0])
			{
				SelectedItem->Accept(FFunctionalItemVisitor()
					.Level([this](const FLevelItem& LevelItem)
					{
						if(LevelItem.Level.IsValid())
						{
							if (ULevel* LoadedLevel = LevelItem.Level->GetLoadedLevel())
							{
								MoveSelectedActorsToLevel(*LoadedLevel);
							}
						}
					})
					.LevelRoot([this](const FLevelRootItem& RootItem)
					{
						if (RootItem.PersistentLevel.IsValid())
						{
							MoveSelectedActorsToLevel(*RootItem.PersistentLevel);
						}
					})
				);
			}
		}
	}

	void STreeViewLevels::MoveSelectedActorsToLevel(ULevel& InLevel)
	{
		if (!GEditor) return;

		if (USelection* ActorSelection = GEditor->GetSelectedActors())
		{
			FScopedTransaction Transaction(LOCTEXT("TransactionMoveSelectedActorsToLevel", "Move Selected Actors To Level."));

			TArray<AActor*> SelectedActors;
			TArray<AActor*> SelectedActorsFiltered;
			ActorSelection->GetSelectedObjects<AActor>(SelectedActors);

			for (AActor* SelectedActor : SelectedActors)
			{
				if (SelectedActor && !LevelStreamingEditor->IsSupportedByEditor(SelectedActor))
				{
					SelectedActorsFiltered.Add(SelectedActor);
				}
			}

			EditorLevelUtils::MoveActorsToLevel(SelectedActorsFiltered, &InLevel);
		}
	}

	void STreeViewLevels::ChangeStreamingMethodClass(UClass* StreamingClass)
	{
		if (!ensure(StreamingClass)) return;

		TArray<TSharedPtr<FBaseItem>> SelectedItems = GetSelectedItems();
		for (TSharedPtr<FBaseItem> SelectedItem : SelectedItems)
		{
			if (SelectedItem.IsValid())
			{
				FFunctionalItemVisitor ChangeStreamingMethodVisitor; 
				ChangeStreamingMethodVisitor = FFunctionalItemVisitor()
					.Level([StreamingClass](const FLevelItem& LevelItem)
					{
						if (LevelItem.Level.IsValid())
						{
							EditorLevelUtils::SetStreamingClassForLevel(LevelItem.Level.Get(), StreamingClass);
						}
					})
					.LevelFolder([&ChangeStreamingMethodVisitor](const FLevelFolderItem& Folder)
					{
						for (TSharedPtr<FBaseItem> SubItem : Folder.Children)
						{
							if (SubItem.IsValid())
							{
								SubItem->Accept(ChangeStreamingMethodVisitor);
							}
						}
					}
				);
				SelectedItem->Accept(ChangeStreamingMethodVisitor);
			}
		}
	}

	ECheckBoxState STreeViewLevels::IsStreamingClassSetTo(UClass* StreamingClass)
	{
		ECheckBoxState CheckState = ECheckBoxState::Unchecked;
		TArray<TSharedPtr<FBaseItem>> SelectedItems = GetSelectedItems();
		for (TSharedPtr<FBaseItem> SelectedItem : SelectedItems)
		{
			CheckState = ECheckBoxState::Unchecked;
			if (SelectedItem.IsValid())
			{
				SelectedItem->Accept(FFunctionalItemVisitor()
					.Level([&CheckState, StreamingClass](const FLevelItem& LevelItem)
					{
						if (LevelItem.Level.IsValid() && LevelItem.Level->GetClass() == StreamingClass)
						{
							CheckState = ECheckBoxState::Checked;
						}
					})
				);
			}
			if (CheckState == ECheckBoxState::Unchecked) break;
		}
		return CheckState;
	}

	void STreeViewLevels::FindLevelsInContentBrowser()
	{
		if (!GEditor) return;

		TArray<UObject*> LevelsToFind;
		LevelsToFind.Reserve(GetNumSelectedItems());

		TArray<TSharedPtr<FBaseItem>> SelectedItems = GetSelectedItems();
		for (TSharedPtr<FBaseItem> SelectedItem : SelectedItems)
		{
			if (SelectedItem.IsValid())
			{
				FFunctionalItemVisitor GetLevelsToFindVisitor; 
				GetLevelsToFindVisitor = FFunctionalItemVisitor()
					.Level([&LevelsToFind](const FLevelItem& LevelItem)
					{
						if (LevelItem.Level.IsValid())
						{
							if (ULevel* Level = LevelItem.Level->GetLoadedLevel())
							{
								if (UObject* Outer = Level->GetOuter())
								{
									LevelsToFind.AddUnique(Outer);
								}
							}
						}
					})
					.LevelRoot([&LevelsToFind](const FLevelRootItem& RootItem)
					{
						if (RootItem.PersistentLevel.IsValid())
						{
							if (UObject* Outer = RootItem.PersistentLevel->GetOuter())
							{
								LevelsToFind.AddUnique(Outer);
							}
						}
					})
					.LevelFolder([&GetLevelsToFindVisitor](const FLevelFolderItem& Folder)
					{
						for (TSharedPtr<FBaseItem> SubItem : Folder.Children)
						{
							if (SubItem.IsValid())
							{
								SubItem->Accept(GetLevelsToFindVisitor);
							}
						}
					}
				);
				SelectedItem->Accept(GetLevelsToFindVisitor);
			}
		}

		GEditor->SyncBrowserToObjects(LevelsToFind);
	}

	void STreeViewLevels::CreateFolder()
	{
		CreateFolderForSelectedItems();
	}

	void STreeViewLevels::CreateSubFolder()
	{
		CreateSubFolderForSelectedItem();
	}

	void STreeViewLevels::SetFoldersNewColor()
	{
		OpenColorPickerForSelectedFolders();
	}

	void STreeViewLevels::RenameItems()
	{
		RenameSelectedItem();
	}

	void STreeViewLevels::DeleteItems()
	{
		FScopedTransaction Transaction(LOCTEXT("TransactionDeleteItems", "Delete Items"));

		TArray<TSharedPtr<FBaseItem>> SelectedItems = GetSelectedItems();

		if (VerifyUserWantsToDeleteLevels(SelectedItems))
		{
			for (TSharedPtr<FBaseItem> SelectedItem : SelectedItems)
			{
				if (SelectedItem.IsValid())
				{
					LevelStreamAssistantUtils::DeleteItem(*SelectedItem);
				}
			}
		}
	}

	bool STreeViewLevels::VerifyUserWantsToDeleteLevels(const TArray<TSharedPtr<FBaseItem>>& ItemsToDelete)
	{
		bool bShouldPrompt = false;

		for (TSharedPtr<FBaseItem> ItemToDelete : ItemsToDelete)
		{
			if (ItemToDelete.IsValid())
			{
				ItemToDelete->Accept(FFunctionalItemVisitor()
					.Level([&bShouldPrompt](const FLevelItem& Level) { bShouldPrompt = true; })
				);

				if (bShouldPrompt) break;
			}
		}

		if (bShouldPrompt)
		{
			FText RemoveLevelWarning = LOCTEXT("RemoveLevelWarning", "Removing levels cannot be undone and any changes to these levels will be lost. Proceed?");
			FText RemoveLevelTitle = LOCTEXT("RemoveLevelTitle", "Remove Level");
			FSuppressableWarningDialog::FSetupInfo Info(RemoveLevelWarning, RemoveLevelTitle, "LevelStreamAssistant_RemoveLevel_Warning");
			Info.ConfirmText = LOCTEXT("RemoveLevelYes", "Yes");
			Info.CancelText = LOCTEXT("RemoveLevelNo", "No");
			FSuppressableWarningDialog WarningDialog(Info);
			bool bConfimed = WarningDialog.ShowModal() != FSuppressableWarningDialog::Cancel;
			return bConfimed;
		}

		return true;
	}

	FFolderManager& STreeViewLevels::GetFolderManager() const
	{
		return FFolderManager::GetLevelFolderManager();
	}
}

#undef LOCTEXT_NAMESPACE