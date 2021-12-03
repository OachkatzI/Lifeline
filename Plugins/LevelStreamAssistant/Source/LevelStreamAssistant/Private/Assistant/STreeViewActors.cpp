// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Assistant/STreeViewActors.h" 
#include <Assistant/TreeItems.h>
#include <LevelStreamAssistantStyle.h>
#include <Assistant/ItemWidgets/SItemLabel.h>
#include <Assistant/ItemWidgets/SItemIcon.h>
#include <Assistant/TreeItemVisitors.h>
#include <ActorIconFinder.h>
#include <Widgets/STreeViewWithColumn.h>
#include <Assistant/LevelStreamingEditor.h>
#include <Utilities/LevelStreamAssistantItemUtils.h>
#include <Iterators/ItemIterators.h>
#include <ScopedTransaction.h>
#include <LevelStreamAssistantCommands.h>
#include <Framework/Commands/UICommandList.h>
#include <Framework/Commands/GenericCommands.h>
#include <Assistant/Folders/FolderManager.h>
#include <Assistant/ItemContextMenuModifier.h>
#include <Assistant/LevelStreamAssistantHierarchyView.h>
#include <Framework/MultiBox/MultiBoxBuilder.h>
#include <LevelStreamingActorEditorBase.h>

#define LOCTEXT_NAMESPACE "LevelStreamAssistant.STreeViewActors"

namespace LevelStreamAssistant
{
	void STreeViewActors::Construct(const FArguments& InArgs, TSharedRef<ILevelStreamingEditor> InLevelStreamingEditor)
	{
		LevelStreamingEditor = InLevelStreamingEditor;

		LevelStreamingEditor->OnSelectedActorsChanged().AddSP(this, &STreeViewActors::OnSelectedActorsChanged);

		ActorIconFinder = MakeShareable(new FActorIconFinder);

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
			.OnGetChildren(this, &STreeViewActors::OnGetRowChildren)
			.OnGetTableRowArgs(this, &STreeViewActors::GetTableRowArgs)
			.OnExpansionChanged(this, &STreeViewActors::OnItemExpansionChanged)
			.OnSelectionChanged(this, &STreeViewActors::OnSelectionChanged)
			.OnItemContextMenuOpening(this, &STreeViewActors::OnItemContextMenuOpening)
			+ STreeViewWithColumn<TSharedPtr<FBaseItem>>::Column()
			.OnGenerateWidget(this, &STreeViewActors::OnGenerateNameWidget)
			.HeaderArgs(SHeaderRow::Column("Name")
				.DefaultLabel(LOCTEXT("NameHeader", "Name"))
				.FillWidth(1)
				[
					SNew(SBox)
					.MinDesiredHeight(22)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock).Text(LOCTEXT("LabelLevelStreamingActors", "Actors"))
					]
				])
		];

		UpdateItemsExpansion();
	}

	STreeViewActors::~STreeViewActors()
	{
		if (LevelStreamingEditor.IsValid())
		{
			LevelStreamingEditor->OnSelectedActorsChanged().RemoveAll(this);
		}
	}

	STableRow<TSharedPtr<FBaseItem>>::FArguments STreeViewActors::GetTableRowArgs(TSharedPtr<FBaseItem> InItem)
	{
		return STableRow<TSharedPtr<FBaseItem>>::FArguments()
			.Style(&FLevelStreamAssistantStyle::Get().GetWidgetStyle<FTableRowStyle>("LSA.ListViewActor.Row"))
			.OnDragDetected(this, &STreeViewActors::OnItemDragDetected)
			.OnDragLeave(this, &STreeViewActors::OnItemDragLeave)
			.OnCanAcceptDrop(this, &STreeViewActors::OnItemCanAcceptDrop)
			.OnAcceptDrop(this, &STreeViewActors::OnItemAcceptDrop);
	}

	void STreeViewActors::OnItemRenamed(const FText& NewName, TSharedPtr<FBaseItem> InItem) const
	{
		if (InItem.IsValid())
		{
			FScopedTransaction Transaction(LOCTEXT("TransactionRenameItem", "Rename Item"));
			LevelStreamAssistantUtils::RenameItem(*InItem, *NewName.ToString());
		}
	}

	TSharedRef<SWidget> STreeViewActors::OnGenerateNameWidget(TSharedRef<ITableRow> Row, TSharedPtr<FBaseItem> InItem)
	{
		return SNew(SBox)
			.MinDesiredHeight(20.f)
			.VAlign(VAlign_Center)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SExpanderArrow, Row)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SItemIcon, Row, InItem.ToSharedRef())
					.ActorIconFinder(ActorIconFinder)
				]
				+ SHorizontalBox::Slot()
				[
					SNew(SItemLabel, Row, InItem.ToSharedRef())
					.OnLabelEdited(this, &STreeViewActors::OnItemRenamed, InItem)
				]
			];
	}

	void STreeViewActors::OnSelectionChanged(TSharedPtr<FBaseItem> Entry, ESelectInfo::Type SelectInfo)
	{
		if (TreeView.IsValid() && !bSelectionChangeExternal)
		{
			LevelStreamingEditor->ClearSelectedLevelStreamingActors();

			FFunctionalItemVisitor AddActor =
				FFunctionalItemVisitor().Actor([&LevelStreamingEditor = LevelStreamingEditor](const FActorItem& ActorItem)
					{
						LevelStreamingEditor->SelectLevelStreamingActor(ActorItem.Actor);
					});


			for (TSharedPtr<FBaseItem> Item : TreeView->GetSelectedItems())
			{
				if (Item.IsValid())
				{
					Item->Accept(AddActor);
				}
			}

			bSelectionChangeInternal = true;
			LevelStreamingEditor->NotifySelectedActorsChanged();
			bSelectionChangeInternal = false;
		}
	}

	void STreeViewActors::OnSelectedActorsChanged()
	{
		if (TreeItemsSource && TreeView.IsValid() && !bSelectionChangeInternal)
		{
			TSet<TWeakObjectPtr<AActor>> SelectedActors(LevelStreamingEditor->GetSelectedLevelStreamingActors());

			TArray<TSharedPtr<FBaseItem>> ItemsToSelect;
			ItemsToSelect.Reserve(SelectedActors.Num());

			FFunctionalItemVisitor SelectItem =
				FFunctionalItemVisitor().Actor([&SelectedActors, &ItemsToSelect](const FActorItem& ActorItem)
					{
						if(SelectedActors.Contains(ActorItem.Actor))
						{
							ItemsToSelect.Add(ConstCastSharedRef<FBaseItem>(ActorItem.AsShared()));
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

	TSharedPtr<SWidget> STreeViewActors::OnItemContextMenuOpening(TSharedPtr<FBaseItem> Item)
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender);
		FMenuBuilder MenuBuilder(true, CommandList, MenuExtender);

		MenuBuilder.BeginSection("Actor", LOCTEXT("LabelActorSection", "Actor"));

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

	bool STreeViewActors::ContextMenuAppliesToAllItems(const FItemContextMenuModifier& ContextMenuModifier, const TArray<TSharedPtr<FBaseItem>>& Items) const
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

	void STreeViewActors::BindCommands()
	{
		const FLevelStreamAssistantCommands& Commands = FLevelStreamAssistantCommands::GetSafe();
		const FGenericCommands& GenericCommands = FGenericCommands::Get();

		CommandList = MakeShareable(new FUICommandList);

		CommandList->MapAction(
			Commands.SetFoldersNewColor,
			FExecuteAction::CreateSP(this, &STreeViewActors::SetFoldersNewColor)
		);

		CommandList->MapAction(
			GenericCommands.Rename,
			FExecuteAction::CreateSP(this, &STreeViewActors::RenameItems)
		);

		CommandList->MapAction(
			GenericCommands.Delete,
			FExecuteAction::CreateSP(this, &STreeViewActors::DeleteItems)
		);

		CommandList->MapAction(
			Commands.CreateFolder,
			FExecuteAction::CreateSP(this, &STreeViewActors::CreateFolder)
		);

		CommandList->MapAction(
			Commands.CreateSubFolder,
			FExecuteAction::CreateSP(this, &STreeViewActors::CreateSubFolder)
		);
	}
	
	void STreeViewActors::PopulateContextMenuModifiers()
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
			.OnModifyMenu([this, &GenericCommands](FExtender& Extender) { ExtendMenuEntry(Extender, "Actor", EExtensionHook::After, GenericCommands.Rename); })
			.AppliesToActor()
			.AppliesToActorFolder()
		);

		AddContextMenuModifier(FItemContextMenuModifier::New()
			.OnModifyMenu([this, &GenericCommands](FExtender& Extender) { ExtendMenuEntry(Extender, "Actor", EExtensionHook::After, GenericCommands.Delete); })
			.SupportsMultipleSelection()
			.AppliesToActor()
			.AppliesToActorFolder()
		);

		AddContextMenuModifier(FItemContextMenuModifier::New()
			.OnModifyMenu([this, &Commands](FExtender& Extender) { ExtendMenuEntry(Extender, "Actor", EExtensionHook::After, Commands.SetFoldersNewColor); })
			.SupportsMultipleSelection()
			.AppliesToActorFolder()
		);

		AddContextMenuModifier(FItemContextMenuModifier::New()
			.OnModifyMenu([this, &Commands](FExtender& Extender) { ExtendMenuEntry(Extender, "Actor", EExtensionHook::After, Commands.CreateFolder); })
			.SupportsMultipleSelection()
			.AppliesToActor()
			.AppliesToActorFolder()
		);

		AddContextMenuModifier(FItemContextMenuModifier::New()
			.OnModifyMenu([this, &Commands](FExtender& Extender) { ExtendMenuEntry(Extender, "Actor", EExtensionHook::After, Commands.CreateSubFolder); })
			.AppliesToActorFolder()
			.AppliesToActorRoot()
		);

		AddContextMenuModifier(FItemContextMenuModifier::New()
			.OnModifyMenu([this](FExtender& Extender) { BuildContextMenuForLevelStreamingEditor(Extender); })
			.SupportsMultipleSelection()
			.AppliesToActor()
		);
	}

	void STreeViewActors::ExtendMenuEntry(FExtender& Extender, const FName& Hook, EExtensionHook::Position Position, TSharedPtr<FUICommandInfo> CommandInfo)
	{
		Extender.AddMenuExtension(Hook, Position, CommandList,
			FMenuExtensionDelegate::CreateLambda([CommandInfo](FMenuBuilder& MenuBuilder) { MenuBuilder.AddMenuEntry(CommandInfo); }));
	};

	void STreeViewActors::BuildContextMenuForLevelStreamingEditor(FExtender& MenuExtender) const
	{
		TArray<TSharedPtr<FBaseItem>> SelectedItems = TreeView->GetSelectedItems();
	
		TArray<AActor*> SelectedActors;
		SelectedActors.Reserve(SelectedItems.Num());
		
		for (TSharedPtr<FBaseItem> SelectedItem : SelectedItems)
		{
			if (SelectedItem.IsValid())
			{
				SelectedItem->Accept(FFunctionalItemVisitor()
					.Actor([&SelectedActors](const FActorItem& ActorItem) 
					{
						if (AActor* SelectedActor = ActorItem.Actor.Get()) 
						{
							SelectedActors.Add(SelectedActor);
						}
					})
				);
			}
		}

		TSharedRef<FLevelStreamAssistantHierarchyView> HierarchyView = MakeShareable(new FLevelStreamAssistantHierarchyView(LevelStreamingEditor));
		LevelStreamingEditor->GetLevelStreamingActorEditor().ExtendActorsContextMenu(MenuExtender, HierarchyView);
	}

	void STreeViewActors::RenameItems()
	{
		RenameSelectedItem();
	}

	void STreeViewActors::DeleteItems()
	{
		FScopedTransaction Transaction(LOCTEXT("TransactionDeleteItems", "Delete Items"));

		TArray<TSharedPtr<FBaseItem>> SelectedItems = GetSelectedItems();
		for (TSharedPtr<FBaseItem> SelectedItem : SelectedItems)
		{
			if (SelectedItem.IsValid())
			{
				LevelStreamAssistantUtils::DeleteItem(*SelectedItem);
			}
		}
	}

	FFolderManager& STreeViewActors::GetFolderManager() const
	{
		return FFolderManager::GetActorFolderManager();
	}

	void STreeViewActors::CreateFolder()
	{
		CreateFolderForSelectedItems();
	}

	void STreeViewActors::CreateSubFolder()
	{
		CreateSubFolderForSelectedItem();
	}

	void STreeViewActors::SetFoldersNewColor()
	{
		OpenColorPickerForSelectedFolders();
	}
}

#undef LOCTEXT_NAMESPACE