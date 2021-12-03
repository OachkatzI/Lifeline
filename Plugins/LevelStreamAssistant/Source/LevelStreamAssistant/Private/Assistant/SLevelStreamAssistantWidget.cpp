// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Assistant/SLevelStreamAssistantWidget.h"
#include "Assistant/TreeItems.h"
#include "Assistant/LevelStreamAssistantWidgetSettings.h"
#include <LevelStreamingActorEditorBase.h>
#include <LevelStreamAssistantCommands.h>
#include "LevelStreamAssistantStyle.h"
#include "Utilities/LevelStreamAssistantUtils.h"
#include "Utilities/LevelStreamAssistantItemUtils.h"
#include "EditorMode/LevelStreamAssistantEdMode.h"
#include <Widgets/SWindow.h>
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SComboButtonStylized.h"
#include "Widgets/SSimpleSearchBox.h"
#include "Widgets/SSizeMapButton.h"
#include "Widgets/SWarning.h"
#include "Widgets/SDragDropActor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Components/BrushComponent.h"
#include "Engine/LevelStreaming.h"
#include "Engine/LevelBounds.h"
#include "Engine/Selection.h"
#include "EngineUtils.h"
#include "Editor.h"
#include "EditorViewportClient.h"
#include "Iterators/WeakPointerIterator.h"
#include <LevelStreamBindingMode.h>
#include "ClassViewerModule.h"
#include <Assistant/STreeViewActors.h>
#include <Assistant/STreeViewLevels.h>
#include <Assistant/LevelStreamingEditor.h>
#include <Assistant/FilteredHierarchy.h>
#include <Assistant/TreeItemVisitors.h>
#include <Assistant/ItemKey.h>
#include <Assistant/SimpleClassFilter.h>
#include <Utilities/LevelStreamingBindingUtils.h>
#include <Iterators/ItemIterators.h>
#include <Assistant/Folders/FolderManager.h>
#include <Scenehighlights.h>

#define LOCTEXT_NAMESPACE "SLevelStreamAssistantWidget"

const FName SLevelStreamAssistantWidget::ActorColumnId = TEXT("Actors");
const FName SLevelStreamAssistantWidget::LevelColumnId = TEXT("Levels");

using namespace LevelStreamAssistant;

void SLevelStreamAssistantWidget::Construct(const FArguments& InArgs)
{
	TSubclassOf<ULevelStreamingActorEditorBase> EditorType = GetSettings().EditorType.LoadSynchronous();

	LevelStreamingEditor = MakeShareable(new FLevelStreamingEditorImpl(EditorType));

	LevelStreamingEditor->OnLevelBindingChanged().AddSP(this, &SLevelStreamAssistantWidget::OnLevelBindingChanged);
	LevelStreamingEditor->OnSelectedActorsChanged().AddSP(this, &SLevelStreamAssistantWidget::OnSelectedActorsChanged);

	GetSettings().TargetType = LevelStreamingEditor->GetLevelStreamingActorEditor().GetTargetType();

	ActorHierarchy = MakeShareable(new FFilteredHierarchy);
	LevelHierarchy = MakeShareable(new FFilteredHierarchy);

	PopulateLists();
	
	// calculate size map at begining.
	MakeUpdateRequest(EUpdateFlags::SizeMap);
	
	// listen to transactions and update list views accordingly.
	// this event is only used to know if level streaming volumes/brushes are added/removed/modified.
	// there might be better replacements for this event.
	FCoreUObjectDelegates::OnObjectTransacted.AddSP(this, &SLevelStreamAssistantWidget::OnObjectTransacted);

	// keep the view updated by listening to these events.
	USelection::SelectionChangedEvent.AddSP(this, &SLevelStreamAssistantWidget::OnEditorSelectionChanged);
	FEditorDelegates::RefreshLevelBrowser.AddSP(this, &SLevelStreamAssistantWidget::OnRefreshLevelBrowser);
	FEditorDelegates::OnMapOpened.AddSP(this, &SLevelStreamAssistantWidget::OnMapOpened);
	FEditorDelegates::EditorModeIDEnter.AddSP(this, &SLevelStreamAssistantWidget::OnEditorModeChanged);

	FFolderManager::GetActorFolderManager().OnFolderRenamed.AddSP(this, &SLevelStreamAssistantWidget::OnFolderRenamed);
	FFolderManager::GetActorFolderManager().OnObjectFolderChanged.AddSP(this, &SLevelStreamAssistantWidget::OnObjectFolderChanged);

	FFolderManager::GetLevelFolderManager().OnFolderRenamed.AddSP(this, &SLevelStreamAssistantWidget::OnFolderRenamed);
	FFolderManager::GetLevelFolderManager().OnObjectFolderChanged.AddSP(this, &SLevelStreamAssistantWidget::OnObjectFolderChanged);

	if (GEditor)
	{
		GEditor->OnActorMoving().AddSP(this, &SLevelStreamAssistantWidget::OnActorMoving);
		GEditor->OnObjectsReplaced().AddSP(this, &SLevelStreamAssistantWidget::OnObjectsReplaced);
		GEditor->RegisterForUndo(this);
	}

	BindCommands();

	TSharedRef<SWidget> OptionsToolbar = BuildOptionsToolbar();

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(4.f)
				[
					SNew(SBox)
					.WidthOverride(32.f)
					.HeightOverride(32.f)
					[
						OptionsToolbar
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(4.f)
				[
					// drag and drop icon
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SDragDropActor)
						.ActorType(this, &SLevelStreamAssistantWidget::GetActorTypeForDragAndDrop)
						.Image(this, &SLevelStreamAssistantWidget::GetIconForActorDragAndDrop)
					]
					// drag and drop type picker
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SAssignNew(ComboButton_TargetTypes, SComboButtonStylized)
						.ContentPadding(0)
						.OnGetMenuContent(FOnGetContent::CreateSP(this, &SLevelStreamAssistantWidget::OnGetMenuContent_TargetType))
						.ToolTipText(LOCTEXT("ToolTipChooseTargetType", "Choose actor type to drag and drop."))
						.ButtonStyle(FEditorStyle::Get(), "ToggleButton")
					]
					// editor type picker
					+ SHorizontalBox::Slot()
					[
						SAssignNew(ComboButton_EditorTypes, SComboButtonStylized)
						.OnGetMenuContent(FOnGetContent::CreateSP(this, &SLevelStreamAssistantWidget::OnGetMenuContent_EditorType))
						.ToolTipText(LOCTEXT("ToolTipChooseEditorType", "Choose an editor to work with."))
						.ButtonStyle(FEditorStyle::Get(), "ToggleButton")
						.ButtonContent()
						[
							SNew(STextBlock).Text(this, &SLevelStreamAssistantWidget::GetEditorName)
						]
					]
				]
			]
			// size map
			+ SHorizontalBox::Slot()
			[
				SAssignNew(SizeMapButton, SSizeMapButton)
				.ToolTipText(LOCTEXT("ToolTipShowSizeMap", "Show size map for levels related to selected actor."))
				.IsOverviewSizeDisplayed(this, &SLevelStreamAssistantWidget::GetIsOverviewSizeDisplayed)
				.GetAssetDataList(this, &SLevelStreamAssistantWidget::GetAssetDataList)
			]
		]
		+ SVerticalBox::Slot()
		[
			SNew(SSplitter)
			.Orientation(EOrientation::Orient_Horizontal)
			+ SSplitter::Slot()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					// search box for actor list
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					[
						SAssignNew(SearchBoxActorItems, SSimpleSearchBox<TSharedPtr<FBaseItem>>)
						.ItemToStringArray_Static(&LevelStreamAssistantUtils::ItemToStringArray)
						.OnSearchChanged(this, &SLevelStreamAssistantWidget::OnSearchChanged_ActorsTree)
					]
					// make new folder
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.ButtonStyle(FEditorStyle::Get(), "ToggleButton")
						.OnClicked(this, &SLevelStreamAssistantWidget::OnClicked_MakeNewFolder_Actors)
						.IsEnabled(this, &SLevelStreamAssistantWidget::CanMakeNewFolder_Actors)
						[
							SNew(SImage).Image(FLevelStreamAssistantStyle::Get().GetBrush("LSA.NewFolderIcon"))
						]
					]
				]
				// actor list
				+ SVerticalBox::Slot()
				[
					SAssignNew(TreeViewActors, STreeViewActors, LevelStreamingEditor.ToSharedRef())
					.TreeItemsSource(&ActorHierarchy->GetHierarchy())
				]
			]
			+ SSplitter::Slot()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					// search box for level list
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					[
						SAssignNew(SearchBoxLevelItems, SSimpleSearchBox<TSharedPtr<FBaseItem>>)
						.ItemToStringArray_Static(&LevelStreamAssistantUtils::ItemToStringArray)
						.OnSearchChanged(this, &SLevelStreamAssistantWidget::OnSearchChanged_LevelsTree)
					]
					// make new folder
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.ButtonStyle(FEditorStyle::Get(), "ToggleButton")
						.OnClicked(this, &SLevelStreamAssistantWidget::OnClicked_MakeNewFolder_Levels)
						.IsEnabled(this, &SLevelStreamAssistantWidget::CanMakeNewFolder_Levels)
						[
							SNew(SImage).Image(FLevelStreamAssistantStyle::Get().GetBrush("LSA.NewFolderIcon"))
						]
					]
				]
				// level list
				+ SVerticalBox::Slot()
				[
					SAssignNew(TreeViewLevels, STreeViewLevels, LevelStreamingEditor.ToSharedRef())
					.TreeItemsSource(&LevelHierarchy->GetHierarchy())
					.CanToggleLevelVisbility(this, &SLevelStreamAssistantWidget::CanToggleLevelVisibility)
					.OnItemHovered(this, &SLevelStreamAssistantWidget::OnLevelHovered)
					.OnItemUnhovered(this, &SLevelStreamAssistantWidget::OnLevelUnhovered)
				]
			]
		]
		+ SVerticalBox::Slot()
		.Padding(2.f, 2.f, 2.f, 0.f)
		.AutoHeight()
		[
			SNew(SWarning)
			.WarningText(LOCTEXT("WarningAutomaticLevelStreaming", "Automatic level streaming is turned on!"))
			.ToolTipText(LOCTEXT("ToolTipWarningAutomaticLevelStreaming", "visibility of levels can not be controlled here as long as automatic level streaming is turned on."))
			.FixText(LOCTEXT("TurnOffAutomaticLevelStreaming", "Turn Off"))
			.DismissText(LOCTEXT("DismissAutomaticLevelStreaming", "Dismiss"))
			.OnFix(this, &SLevelStreamAssistantWidget::FixWarning_AutomaticLevelStreaming)
			.ShouldBeFixed(this, &SLevelStreamAssistantWidget::ShouldBeFixed_AutomaticLevelStreaming)
		]
		+ SVerticalBox::Slot()
		.Padding(2.f)
		.AutoHeight()
		[
			SNew(SWarning)
			.WarningText(LOCTEXT("WarningViewportRealtimeRendering", "Viewport realtime rendering is turned off!"))
			.ToolTipText(LOCTEXT("ToolTipWarningViewportRealtimeRendering", "some features may not work correctly. turn on viewport realtime rendering for better experience."))
			.FixText(LOCTEXT("TurnOnViewportRealtimeRendering", "Turn On"))
			.DismissText(LOCTEXT("DismissViewportRealtimeRendering", "Dismiss"))
			.OnFix(this, &SLevelStreamAssistantWidget::FixWarning_ViewportRealtimeRendering)
			.ShouldBeFixed(this, &SLevelStreamAssistantWidget::ShouldBeFixed_ViewportRealtimeRendering)
		]
	];
}

SLevelStreamAssistantWidget::SLevelStreamAssistantWidget()
{
	bHoveredLevelWasVisible = false;
	bSortLevelsAscending = true;
	bSortActorsAscending = true;
}

SLevelStreamAssistantWidget::~SLevelStreamAssistantWidget()
{
	// save user settings before exit.
	GetSettings().SaveConfig();
	
	if (LevelStreamingEditor.IsValid())
	{
		LevelStreamingEditor->GetLevelStreamingActorEditor().SaveConfig();

		LevelStreamingEditor->OnLevelBindingChanged().RemoveAll(this);
		LevelStreamingEditor->OnSelectedActorsChanged().RemoveAll(this);
	}

	FCoreUObjectDelegates::OnObjectTransacted.RemoveAll(this);
	USelection::SelectionChangedEvent.RemoveAll(this);
	FEditorDelegates::RefreshLevelBrowser.RemoveAll(this);
	FEditorDelegates::OnMapOpened.RemoveAll(this);
	FEditorDelegates::EditorModeIDEnter.RemoveAll(this);

	FFolderManager::GetActorFolderManager().OnFolderRenamed.RemoveAll(this);
	FFolderManager::GetActorFolderManager().OnObjectFolderChanged.RemoveAll(this);
	FFolderManager::GetLevelFolderManager().OnFolderRenamed.RemoveAll(this);
	FFolderManager::GetLevelFolderManager().OnObjectFolderChanged.RemoveAll(this);

	if (GEditor)
	{
		GEditor->OnActorMoving().RemoveAll(this);
		GEditor->OnObjectsReplaced().RemoveAll(this);
		GEditor->UnregisterForUndo(this);
	}
}

void SLevelStreamAssistantWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (IsUpdateRequestMade())
	{
		UpdateViewport();
		ClearUpdateRequest();
	}
}

void SLevelStreamAssistantWidget::BindCommands()
{
	CommandList = MakeShareable(new FUICommandList);
	
	const FLevelStreamAssistantCommands& Commands = FLevelStreamAssistantCommands::GetSafe();

	CommandList->MapAction(Commands.Option_ShowOnlyRelatedLevels, FUIAction(
		FExecuteAction::CreateSP(this, &SLevelStreamAssistantWidget::ExecuteAction_ShowOnlyRelatedLevels),
		FCanExecuteAction::CreateSP(this, &SLevelStreamAssistantWidget::CanControlLevelVisibility),
		FIsActionChecked::CreateSP(this, &SLevelStreamAssistantWidget::ShouldShowOnlyRelatedLevels)
	));

	CommandList->MapAction(Commands.Option_HighlightRelatedLevels, FUIAction(
		FExecuteAction::CreateSP(this, &SLevelStreamAssistantWidget::ExecuteAction_HighlightRelatedLevels),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &SLevelStreamAssistantWidget::ShouldHighlightRelatedLevels)
	));

	CommandList->MapAction(Commands.Option_FocusBoundLevels, FUIAction(
		FExecuteAction::CreateSP(this, &SLevelStreamAssistantWidget::ExecuteAction_FocusOnBoundLevels),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &SLevelStreamAssistantWidget::ShouldFocusOnBoundLevels)
	));

	CommandList->MapAction(Commands.Option_FocusSelectedActor, FUIAction(
		FExecuteAction::CreateSP(this, &SLevelStreamAssistantWidget::ExecuteAction_FocusOnSelectedActor),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &SLevelStreamAssistantWidget::ShouldFocusOnSelectedActor)
	));

	CommandList->MapAction(Commands.Option_SyncWithViewportSelection, FUIAction(
		FExecuteAction::CreateSP(this, &SLevelStreamAssistantWidget::ExecuteAction_SyncListSelectionWithViewportSelection),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &SLevelStreamAssistantWidget::ShouldSyncListSelectionWithViewportSelection)
	));

	CommandList->MapAction(Commands.Option_PreviewLevelVisibility, FUIAction(
		FExecuteAction::CreateSP(this, &SLevelStreamAssistantWidget::ExecuteAction_PreviewLevelVisibility),
		FCanExecuteAction::CreateSP(this, &SLevelStreamAssistantWidget::CanExecuteAction_PreviewLevelVisibility),
		FIsActionChecked::CreateSP(this, &SLevelStreamAssistantWidget::ShouldPreviewLevelVisibility)
	));

	CommandList->MapAction(Commands.Option_HighlightSelections, FUIAction(
		FExecuteAction::CreateSP(this, &SLevelStreamAssistantWidget::ExecuteAction_HighlightSelections),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &SLevelStreamAssistantWidget::ShouldHighlightSelections)
	));

	CommandList->MapAction(Commands.Option_PreviewSizeMap, FUIAction(
		FExecuteAction::CreateSP(this, &SLevelStreamAssistantWidget::ExecuteAction_CalculateSizeMap),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &SLevelStreamAssistantWidget::ShouldCalculateSizeMap)
	));

	CommandList->MapAction(Commands.OpenEditorDetailsPanel, FUIAction(
		FExecuteAction::CreateSP(this, &SLevelStreamAssistantWidget::OpenEditorDetailsPanel)
	));
}

TSharedRef<SWidget> SLevelStreamAssistantWidget::BuildOptionsToolbar() const
{
	const FLevelStreamAssistantCommands& Commands = FLevelStreamAssistantCommands::GetSafe();

	FToolBarBuilder OptionsToolbarBuilder = FToolBarBuilder(CommandList, FMultiBoxCustomization::None);
	OptionsToolbarBuilder.SetLabelVisibility(EVisibility::Collapsed);

	OptionsToolbarBuilder.AddToolBarButton(Commands.Option_ShowOnlyRelatedLevels);
	OptionsToolbarBuilder.AddToolBarButton(Commands.Option_HighlightRelatedLevels);
	OptionsToolbarBuilder.AddSeparator();
	OptionsToolbarBuilder.AddToolBarButton(Commands.Option_FocusBoundLevels);
	OptionsToolbarBuilder.AddToolBarButton(Commands.Option_FocusSelectedActor);
	OptionsToolbarBuilder.AddSeparator();
	OptionsToolbarBuilder.AddToolBarButton(Commands.Option_SyncWithViewportSelection);
	
	OptionsToolbarBuilder.AddComboButton(FUIAction(),
		FOnGetContent::CreateLambda([&CommandList = CommandList, &Commands]()
		{
			FMenuBuilder MenuBuilder(true, CommandList);
			MenuBuilder.AddMenuEntry(Commands.Option_PreviewLevelVisibility);
			MenuBuilder.AddMenuEntry(Commands.Option_HighlightSelections);
			MenuBuilder.AddMenuEntry(Commands.Option_PreviewSizeMap);
			MenuBuilder.AddMenuEntry(Commands.OpenEditorDetailsPanel);
			return MenuBuilder.MakeWidget();
		}),
		LOCTEXT("LabelMoreOptions", "More Options"),
		/*ToolTip*/FText::GetEmpty(),
		FSlateIcon(FLevelStreamAssistantStyle::GetStyleSetName(), "LSA.Commands.Settings")
	);

	return OptionsToolbarBuilder.MakeWidget();
}

void SLevelStreamAssistantWidget::OnSearchChanged_ActorsTree()
{
	if (ActorHierarchy.IsValid() && TreeViewActors.IsValid() && SearchBoxActorItems.IsValid())
	{
		ActorHierarchy->FilterHierarchy(FItemPassesFilter::CreateSP(SearchBoxActorItems.ToSharedRef(), &SSimpleSearchBox<TSharedPtr<FBaseItem>>::PassesFilter));

		TreeViewActors->RequestTreeRefresh();
	}
}

void SLevelStreamAssistantWidget::OnSearchChanged_LevelsTree()
{
	if (LevelHierarchy.IsValid() && TreeViewLevels.IsValid() && SearchBoxLevelItems.IsValid())
	{
		LevelHierarchy->FilterHierarchy(FItemPassesFilter::CreateSP(SearchBoxLevelItems.ToSharedRef(), &SSimpleSearchBox<TSharedPtr<FBaseItem>>::PassesFilter));

		TreeViewLevels->RequestTreeRefresh();
	}
}

// when level is hovered in listview, highlight what is going to be added or removed.
void SLevelStreamAssistantWidget::OnHovered_Level(TWeakObjectPtr<ULevelStreaming> InLevel)
{
	if (HoveredLevel != InLevel)
	{
		OnUnhovered_Level(HoveredLevel); // make sure last hovered level is unhovered.

		HoveredLevel = InLevel;

		if (ULevelStreaming* Level = InLevel.Get())
		{
			const bool bIsLevelVisible = Level->IsLevelVisible();

			if (ShouldPreviewLevelVisibility() && ShouldShowOnlyRelatedLevels() && CanControlLevelVisibility())
			{
				// if binding is set, it would be cleared after level is clicked.
				bool bPreviewVisible = !IsLevelBoundToAnySelectedActor(LevelStreamingEditor, Level);

				// set visibility for preview
				if (bPreviewVisible != bIsLevelVisible)
				{
					Level->SetShouldBeVisibleInEditor(bPreviewVisible);
					LevelStreamAssistantUtils::FlushLevelStreaming();
				}
			}

			bHoveredLevelWasVisible = bIsLevelVisible;
		}

		MakeUpdateRequest(EUpdateFlags::Highlight);
	}
}

// when level is unhovered, reset level visibility.
void SLevelStreamAssistantWidget::OnUnhovered_Level(TWeakObjectPtr<ULevelStreaming> InLevel)
{
	if (HoveredLevel == InLevel)
	{
		if (ULevelStreaming* Level = InLevel.Get())
		{
			if (ShouldPreviewLevelVisibility() && ShouldShowOnlyRelatedLevels() && CanControlLevelVisibility())
			{
				// restore visibility of hovered level.
				if (bHoveredLevelWasVisible != Level->IsLevelVisible())
				{
					Level->SetShouldBeVisibleInEditor(bHoveredLevelWasVisible);
					LevelStreamAssistantUtils::FlushLevelStreaming();
				}
			}
		}

		HoveredLevel.Reset();

		MakeUpdateRequest(EUpdateFlags::Highlight);
	}
}

void SLevelStreamAssistantWidget::OnLevelHovered(TSharedPtr<LevelStreamAssistant::FBaseItem> LevelItem)
{
	if (LevelItem.IsValid())
	{
		LevelItem->Accept(FFunctionalItemVisitor()
			.Level([this](const FLevelItem& Item) { OnHovered_Level(Item.Level); })
		);
	}
}

void SLevelStreamAssistantWidget::OnLevelUnhovered(TSharedPtr<LevelStreamAssistant::FBaseItem> LevelItem)
{
	if (LevelItem.IsValid())
	{
		LevelItem->Accept(FFunctionalItemVisitor()
			.Level([this](const FLevelItem& Item) { OnUnhovered_Level(Item.Level); })
		);
	}
}

bool SLevelStreamAssistantWidget::CanToggleLevelVisibility() const
{
	return !ShouldShowOnlyRelatedLevels() && CanControlLevelVisibility();
}

void SLevelStreamAssistantWidget::OnLevelBindingChanged()
{
	MakeUpdateRequest(EUpdateFlags::LevelBinding);
}

void SLevelStreamAssistantWidget::OnSelectedActorsChanged()
{
	MakeUpdateRequest(EUpdateFlags::LevelBinding | EUpdateFlags::ViewportSelection);
}

FReply SLevelStreamAssistantWidget::OnClicked_MakeNewFolder_Actors() const
{
	if (TreeViewActors.IsValid())
	{
		TreeViewActors->CreateFolderForSelectedItems();
	}
	return FReply::Handled();
}

bool SLevelStreamAssistantWidget::CanMakeNewFolder_Actors() const
{
	return TreeViewActors.IsValid();
}

FReply SLevelStreamAssistantWidget::OnClicked_MakeNewFolder_Levels() const
{
	if (TreeViewLevels.IsValid())
	{
		TreeViewLevels->CreateFolderForSelectedItems();
	}
	return FReply::Handled();
}

bool SLevelStreamAssistantWidget::CanMakeNewFolder_Levels() const
{
	return TreeViewLevels.IsValid();
}

TSharedRef<SWidget> SLevelStreamAssistantWidget::OnGetMenuContent_EditorType()
{
	TSharedPtr<FSimpleClassFilter> ClassFilter = 
		MakeShareable(new FSimpleClassFilter({ ULevelStreamingActorEditorBase::StaticClass() }));

	FClassViewerModule& ClassPicker = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.ClassFilter = ClassFilter;
	return ClassPicker.CreateClassViewer(Options, FOnClassPicked::CreateSP(this, &SLevelStreamAssistantWidget::OnEditorTypePicked));
}

TSharedRef<SWidget> SLevelStreamAssistantWidget::OnGetMenuContent_TargetType()
{
	TSharedPtr<FSimpleClassFilter> ClassFilter =
		MakeShareable(new FSimpleClassFilter({ LevelStreamingEditor->GetLevelStreamingActorEditor().GetTargetType() }));

	FClassViewerModule& ClassPicker = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.ClassFilter = ClassFilter;
	Options.bIsActorsOnly = true;
	return ClassPicker.CreateClassViewer(Options, FOnClassPicked::CreateSP(this, &SLevelStreamAssistantWidget::OnTargetTypePicked));
}

TSubclassOf<AActor> SLevelStreamAssistantWidget::GetActorTypeForDragAndDrop() const
{
	TSubclassOf<AActor> ChosenType = GetSettings().TargetType;
	TSubclassOf<AActor> DefaultType = LevelStreamingEditor->GetLevelStreamingActorEditor().GetTargetType();

	if (!ChosenType || !ChosenType->IsChildOf(DefaultType))
	{
		GetSettings().TargetType = DefaultType;
		ChosenType = DefaultType;
	}

	return ChosenType;
}

const FSlateBrush* SLevelStreamAssistantWidget::GetIconForActorDragAndDrop() const
{
	return LevelStreamingEditor->GetLevelStreamingActorEditor().GetDragAndDropActorIcon();
}

bool SLevelStreamAssistantWidget::GetIsOverviewSizeDisplayed() const
{
	return ShouldCalculateSizeMap();
}

TArray<FAssetData> SLevelStreamAssistantWidget::GetAssetDataList() const
{
	TArray<FAssetData> AssetDataList; // list of assets to show size map for.

	// add persistent level
	if (UWorld* World = LevelStreamAssistantUtils::GetWorldFromEditor())
	{
		AssetDataList.Add(World->PersistentLevel);
	}
	 
	// add streaming levels
	for (ULevelStreaming* StreamingLevel : TWeakArraySafeRange<ULevelStreaming>(ListLevels))
	{
		if (IsLevelBoundToAnySelectedActor(LevelStreamingEditor, StreamingLevel))
		{
			AssetDataList.Add(StreamingLevel->GetLoadedLevel());
		}
	}

	return AssetDataList;
}

void SLevelStreamAssistantWidget::ExecuteAction_ShowOnlyRelatedLevels()
{
	GetSettings().bShowOnlyRelatedLevels = !ShouldShowOnlyRelatedLevels();

	if (ShouldShowOnlyRelatedLevels())
	{
		MakeUpdateRequest(EUpdateFlags::LevelVisibility | EUpdateFlags::ViewportFocus);
	}
	else
	{
		// make all levels visible.
		for (TWeakObjectPtr<ULevelStreaming> LevelStreaming : ListLevels)
		{
			LevelStreaming->SetShouldBeVisibleInEditor(true);
		}
		LevelStreamAssistantUtils::FlushLevelStreaming();
	}
}

void SLevelStreamAssistantWidget::ExecuteAction_HighlightRelatedLevels()
{
	GetSettings().bHighlightRelatedLevels = !ShouldHighlightRelatedLevels();
	MakeUpdateRequest(EUpdateFlags::Highlight);
}

void SLevelStreamAssistantWidget::ExecuteAction_FocusOnBoundLevels()
{
	GetSettings().bFocusOnBoundLevels = !ShouldFocusOnBoundLevels();

	MakeUpdateRequest(EUpdateFlags::ViewportFocus);

}

void SLevelStreamAssistantWidget::ExecuteAction_FocusOnSelectedActor()
{
	GetSettings().bFocusOnSelectedActor = !ShouldFocusOnSelectedActor();

	MakeUpdateRequest(EUpdateFlags::ViewportFocus);
}

void SLevelStreamAssistantWidget::ExecuteAction_SyncListSelectionWithViewportSelection()
{
	GetSettings().bSyncListSelectionWithViewportSelection = !ShouldSyncListSelectionWithViewportSelection();

	if (ShouldSyncListSelectionWithViewportSelection())
	{
		MakeUpdateRequest(EUpdateFlags::ViewportSelection);
	}
}

void SLevelStreamAssistantWidget::ExecuteAction_PreviewLevelVisibility()
{
	GetSettings().bPreviewLevelVisibility = !ShouldPreviewLevelVisibility();
}

void SLevelStreamAssistantWidget::ExecuteAction_HighlightSelections()
{
	GetSettings().bHighlightSelections = !ShouldHighlightSelections();
	MakeUpdateRequest(EUpdateFlags::Highlight);
}

void SLevelStreamAssistantWidget::ExecuteAction_CalculateSizeMap()
{
	GetSettings().bCalculateSizeMap = !ShouldCalculateSizeMap();
	MakeUpdateRequest(EUpdateFlags::SizeMap);
}

void SLevelStreamAssistantWidget::OpenEditorDetailsPanel()
{
	if (FSlateApplication::IsInitialized())
	{
		if (TSharedPtr<SWindow> OpenedEditorDetailsPanel = OpenedEditorDetailsPanelWeakPtr.Pin())
		{
			OpenedEditorDetailsPanel->RequestDestroyWindow();
			OpenedEditorDetailsPanelWeakPtr.Reset();
		}

		FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

		FDetailsViewArgs DetailsViewArgs(
			/*bUpdateFromSelection=*/ false,
			/*bLockable=*/ false,
			/*bAllowSearch=*/ true,
			/*InNameAreaSettings=*/ FDetailsViewArgs::HideNameArea,
			/*bHideSelectionTip=*/ true);

		DetailsViewArgs.bShowPropertyMatrixButton = false;
		DetailsViewArgs.bShowOptions = false;

		TSharedRef<IDetailsView> EditorDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

		TWeakObjectPtr<ULevelStreamingActorEditorBase> CustomizedEditorWeakPtr = &LevelStreamingEditor->GetLevelStreamingActorEditor();

		EditorDetailsView->SetObject(&LevelStreamingEditor->GetLevelStreamingActorEditor());
		EditorDetailsView->OnFinishedChangingProperties().AddSP(this, &SLevelStreamAssistantWidget::OnFinishedChangingEditorProperties);

		FText WindowTitle = FText::Format(LOCTEXT("WindowTitle", "{0} Details"), GetEditorName());
		
		TSharedRef<SWindow> EditorDetailsPanelWindow = SNew(SWindow)
			.Title(WindowTitle)
			.AutoCenter(EAutoCenter::PreferredWorkArea)
			.SizingRule(ESizingRule::UserSized)
			.ClientSize(FVector2D(300.f, 400.f))
			.SupportsMaximize(false)
			.SupportsMinimize(false);

		EditorDetailsPanelWindow->SetContent(EditorDetailsView);
		EditorDetailsPanelWindow->GetOnWindowClosedEvent().AddSP(this, &SLevelStreamAssistantWidget::OnEditorDetailsPanelClosed, CustomizedEditorWeakPtr);
		
		TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(SharedThis(this));
		if (ParentWindow.IsValid())
		{
			OpenedEditorDetailsPanelWeakPtr = FSlateApplication::Get().AddWindowAsNativeChild(EditorDetailsPanelWindow, ParentWindow.ToSharedRef());
		}
		else
		{
			OpenedEditorDetailsPanelWeakPtr = FSlateApplication::Get().AddWindow(EditorDetailsPanelWindow);
		}
	}
}

void SLevelStreamAssistantWidget::OnEditorDetailsPanelClosed(const TSharedRef<SWindow>& Window, TWeakObjectPtr<ULevelStreamingActorEditorBase> InCustomizedEditor)
{
	if (InCustomizedEditor.IsValid())
	{
		InCustomizedEditor->SaveConfig();
	}

	Window->GetOnWindowClosedEvent().RemoveAll(this);
}

void SLevelStreamAssistantWidget::OnFinishedChangingEditorProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
	MakeUpdateRequest(EUpdateFlags::Highlight | EUpdateFlags::RebuildLevelList);
}

bool SLevelStreamAssistantWidget::CanExecuteAction_PreviewLevelVisibility() const
{
	return ShouldShowOnlyRelatedLevels() && CanControlLevelVisibility();
}

bool SLevelStreamAssistantWidget::ShouldShowOnlyRelatedLevels() const
{
	return GetSettings().bShowOnlyRelatedLevels;
}

bool SLevelStreamAssistantWidget::ShouldHighlightRelatedLevels() const
{
	return GetSettings().bHighlightRelatedLevels;
}

bool SLevelStreamAssistantWidget::ShouldFocusOnBoundLevels() const
{
	return GetSettings().bFocusOnBoundLevels;
}

bool SLevelStreamAssistantWidget::ShouldFocusOnSelectedActor() const
{
	return GetSettings().bFocusOnSelectedActor;
}

bool SLevelStreamAssistantWidget::ShouldSyncListSelectionWithViewportSelection() const
{
	return GetSettings().bSyncListSelectionWithViewportSelection;
}

bool SLevelStreamAssistantWidget::ShouldPreviewLevelVisibility() const
{
	return GetSettings().bPreviewLevelVisibility;
}

bool SLevelStreamAssistantWidget::ShouldHighlightSelections() const
{
	return GetSettings().bHighlightSelections;
}

bool SLevelStreamAssistantWidget::ShouldCalculateSizeMap() const
{
	return GetSettings().bCalculateSizeMap;
}

bool SLevelStreamAssistantWidget::CanControlLevelVisibility() const
{
	// if automatic level streaming is active, we should not control level visibility.
	ULevelEditorViewportSettings* ViewportSettings = GetMutableDefault<ULevelEditorViewportSettings>();
	return !ViewportSettings || !ViewportSettings->bLevelStreamingVolumePrevis;
}

void SLevelStreamAssistantWidget::OnObjectTransacted(UObject* Object, const class FTransactionObjectEvent& TransactionObjectEvent)
{
	if (!Object) return;

	bool bShouldRefresh = LevelStreamingEditor->IsSupportedByEditor(Cast<AActor>(Object));
	bShouldRefresh = bShouldRefresh || Cast<UActorComponent>(Object) && LevelStreamingEditor->IsSupportedByEditor(Cast<UActorComponent>(Object)->GetOwner());
	if(bShouldRefresh)
	{
		RefreshView();

		// retract unwanted updates in this event.
		RetractUpdateRequest(EUpdateFlags::ViewportFocus | EUpdateFlags::ViewportSelection | EUpdateFlags::SizeMap);
	}
	else if (Object->IsA<UBrushComponent>())
	{
		MakeUpdateRequest(EUpdateFlags::Highlight);
	}
}

// sync viewport selection with actor list selection.
void SLevelStreamAssistantWidget::OnEditorSelectionChanged(UObject* NewSelection)
{
	if (!ShouldSyncListSelectionWithViewportSelection()) return;

	// after drag and droping a new level streaming actor, this event could happen before lists are repopulated, so list selection should be updated on next tick.
	MakeUpdateRequest(EUpdateFlags::ListSelection);
}

void SLevelStreamAssistantWidget::OnRefreshLevelBrowser()
{
	// levels might be added/removed, refresh the view.
	RefreshView();
}

void SLevelStreamAssistantWidget::OnMapOpened(const FString& MapName, bool bAsTemplate)
{
	// refresh the view when a new map is opened.
	RefreshView();
}

void SLevelStreamAssistantWidget::OnEditorModeChanged(const FEditorModeID& ModeId)
{
	if (ModeId == FLevelStreamAssistantEdMode::EM_LevelStreamAssistantEdModeId)
	{
		// update because actor selection might have been changed outside our plugin mode.
		MakeUpdateRequest(EUpdateFlags::Highlight);
	}
}

void SLevelStreamAssistantWidget::OnFolderRenamed(const FName& OldName, const FName& NewName)
{
	RefreshView();
}

void SLevelStreamAssistantWidget::OnObjectFolderChanged(const UObject* InActor, const FName& OldPath, const FName& NewPath)
{
	RefreshView();
}

void SLevelStreamAssistantWidget::OnObjectsReplaced(const TMap<UObject*, UObject*>& ReplacementMap)
{
	if (ReplacementMap.Find(&LevelStreamingEditor->GetLevelStreamingActorEditor()))
	{
		RefreshView();
		return;
	}

	for (AActor* Actor : TWeakArraySafeRange<AActor>(ListActors))
	{
		if (ReplacementMap.Find(Actor))
		{
			RefreshView();
			break;
		}
	}
}

void SLevelStreamAssistantWidget::OnActorMoving(AActor* MovingActor)
{
	if (LevelStreamingEditor->IsSupportedByEditor(MovingActor))
	{
		MakeUpdateRequest(EUpdateFlags::Highlight);
	}
}

void SLevelStreamAssistantWidget::PostUndo(bool bSuccess)
{
	RefreshView();
}

void SLevelStreamAssistantWidget::PostRedo(bool bSuccess)
{
	RefreshView();
}

bool SLevelStreamAssistantWidget::IsAnyActorSelected() const
{
	return LevelStreamingEditor->GetSelectedLevelStreamingActors().Num() > 0;
}

void SLevelStreamAssistantWidget::OnColumnSortModeChanged(const EColumnSortPriority::Type SortPriority, const FName& ColumnId, const EColumnSortMode::Type InSortMode)
{
	if (ColumnId == SLevelStreamAssistantWidget::ActorColumnId)
	{
		bSortActorsAscending = !bSortActorsAscending;

		SortActors();

		if (TreeViewActors.IsValid())
		{
			TreeViewActors->RequestTreeRefresh();
		}
	}
	else if (ColumnId == SLevelStreamAssistantWidget::LevelColumnId)
	{
		bSortLevelsAscending = !bSortLevelsAscending;

		SortLevels();

		if (TreeViewLevels.IsValid())
		{
			TreeViewLevels->RequestTreeRefresh();
		}
	}
}

void SLevelStreamAssistantWidget::SortLevels()
{
	if (LevelHierarchy.IsValid())
	{
		LevelHierarchy->Sort([bAscending = bSortLevelsAscending](TSharedPtr<FBaseItem> x, TSharedPtr<FBaseItem> y)
		{
			return LevelStreamAssistantUtils::CompareItems(x, y, bAscending == 1);
		});
	}

	ListLevels.Sort([bAscending = bSortLevelsAscending](const TWeakObjectPtr<ULevelStreaming>& x, const TWeakObjectPtr<ULevelStreaming>& y)
	{ 
		return LevelStreamAssistantUtils::CompareLevelNames(x, y, bAscending == 1);
	});
}

void SLevelStreamAssistantWidget::SortActors()
{
	if (ActorHierarchy.IsValid())
	{
		ActorHierarchy->Sort([bAscending = bSortActorsAscending](TSharedPtr<FBaseItem> x, TSharedPtr<FBaseItem> y)
		{
			return LevelStreamAssistantUtils::CompareItems(x, y, bAscending == 1);
		});
	}

	ListActors.Sort([bAscending = bSortActorsAscending](const TWeakObjectPtr<AActor>& x, const TWeakObjectPtr<AActor>& y)
	{ 
		return LevelStreamAssistantUtils::CompareActorNames(x, y, bAscending == 1);
	});
}

void SLevelStreamAssistantWidget::FixWarning_AutomaticLevelStreaming()
{
	if (ULevelEditorViewportSettings* ViewportSettings = GetMutableDefault<ULevelEditorViewportSettings>())
	{
		ViewportSettings->bLevelStreamingVolumePrevis = false;
		ViewportSettings->PostEditChange();
	}
}

bool SLevelStreamAssistantWidget::ShouldBeFixed_AutomaticLevelStreaming() const
{
	// if automatic level streaming is active we can't control level visibility and this should be fixed.
	return !CanControlLevelVisibility();
}

void SLevelStreamAssistantWidget::FixWarning_ViewportRealtimeRendering()
{
	if (GEditor)
	{
		if (FViewport* Viewport = GEditor->GetActiveViewport())
		{
			if (FEditorViewportClient* Client = (FEditorViewportClient*)Viewport->GetClient())
			{
				Client->SetRealtime(true);
			}
		}
	}
}

bool SLevelStreamAssistantWidget::ShouldBeFixed_ViewportRealtimeRendering() const
{
	if (GEditor)
	{
		if (FViewport* Viewport = GEditor->GetActiveViewport())
		{
			if (FEditorViewportClient* Client = (FEditorViewportClient*)Viewport->GetClient())
			{
				// if realtime viewport rendering is not active, some features may not work properly and this should be fixed.
				return !Client->IsRealtime();
			}
		}
	}
	return false;
}

void SLevelStreamAssistantWidget::RefreshView()
{
	MakeUpdateRequest(EUpdateFlags::PopulateLists | EUpdateFlags::LevelVisibility | EUpdateFlags::Highlight | EUpdateFlags::SizeMap | EUpdateFlags::RebuildLists);
}

void SLevelStreamAssistantWidget::PopulateLists()
{
	if (UWorld* World = LevelStreamAssistantUtils::GetWorldFromEditor())
	{
		// get all available streaming levels.
		ListLevels = TArray<TWeakObjectPtr<ULevelStreaming>>(World->GetStreamingLevels());
		
		// get list of all supported level streaming actors from editor world.
		ListActors.Empty(ListLevels.Num());

		TActorRange<AActor> IterateValidActorsIncludingHiddenActors(World, AActor::StaticClass(), EActorIteratorFlags::SkipPendingKill);
		for (AActor* Actor : IterateValidActorsIncludingHiddenActors)
		{
			if (IsValid(Actor) && LevelStreamingEditor->IsSupportedByEditor(Actor))
			{
				ListActors.Add(Actor);
			}
		}

		PopulateActorTree();
		PopulateLevelTree();

		LevelStreamingEditor->SetLevelStreamingActors(ListActors);
		LevelStreamingEditor->SetStreamingLevels(ListLevels);
	}
}

void SLevelStreamAssistantWidget::PopulateActorTree()
{
	TArray<FItemKey> ActorItemKeys;
	for (AActor* Actor : TWeakArraySafeRange<AActor>(ListActors))
	{
		ActorItemKeys.Add(FItemKey(Actor));
	}

	if (UWorld* World = LevelStreamAssistantUtils::GetWorldFromEditor())
	{
		ActorItemKeys.Add(FItemKey(World));

		const TArray<FName>& AllFolders = FFolderManager::GetActorFolderManager().GetAllFolders(*World);
		for (const FName& Folder : AllFolders)
		{
			ActorItemKeys.Add(FItemKey(Folder, /*bIsLevel*/ false));
		}
	}

	FItemPassesFilter ActorItemPassesFilter;
	if (SearchBoxActorItems.IsValid())
	{
		ActorItemPassesFilter = FItemPassesFilter::CreateSP(SearchBoxActorItems.ToSharedRef(), &SSimpleSearchBox<TSharedPtr<FBaseItem>>::PassesFilter);
	}

	if (ActorHierarchy.IsValid()) ActorHierarchy->ResetItems(ActorItemKeys, ActorItemPassesFilter);
	SortActors();
	if (TreeViewActors.IsValid()) TreeViewActors->RequestTreeRefresh();
}

void SLevelStreamAssistantWidget::PopulateLevelTree()
{
	TArray<FItemKey> LevelItemKeys;
	for (ULevelStreaming* Level : TWeakArraySafeRange<ULevelStreaming>(ListLevels))
	{
		LevelItemKeys.Add(FItemKey(Level));
	}

	if (UWorld* World = LevelStreamAssistantUtils::GetWorldFromEditor())
	{
		LevelItemKeys.Add(FItemKey(World->PersistentLevel));

		const TArray<FName>& AllFolders = FFolderManager::GetLevelFolderManager().GetAllFolders(*World);
		for (const FName& Folder : AllFolders)
		{
			LevelItemKeys.Add(FItemKey(Folder, /*bIsLevel*/ true));
		}
	}

	FItemPassesFilter LevelItemPassesFilter;
	if (SearchBoxLevelItems.IsValid())
	{
		LevelItemPassesFilter = FItemPassesFilter::CreateSP(SearchBoxLevelItems.ToSharedRef(), &SSimpleSearchBox<TSharedPtr<FBaseItem>>::PassesFilter);
	}

	if (LevelHierarchy.IsValid()) LevelHierarchy->ResetItems(LevelItemKeys, LevelItemPassesFilter);
	SortLevels();
	if (TreeViewLevels.IsValid()) TreeViewLevels->RequestTreeRefresh();
}

void SLevelStreamAssistantWidget::UpdateViewport()
{
	if (!GEditor) return;
	
	// we don't make any transactions here because we want to keep redo history for the user.
	// this has a side effect which is history for editor selection might be changed.
	// however, we prefer loosing editor selection history than loosing entire redo history.

	if (IsUpdateRequestMadeFor(EUpdateFlags::PopulateLists))
	{
		PopulateLists();
	}

	if (IsUpdateRequestMadeFor(EUpdateFlags::ListSelection))
	{
		// this should be updated first since other updates may depend on SelectedLevelStreamingActors array.
		UpdateListSelection();
	}

	if(IsUpdateRequestMadeFor(EUpdateFlags::LevelVisibility))
	{
		UpdateLevelVisibility();
	}

	if(IsUpdateRequestMadeFor(EUpdateFlags::ViewportFocus))
	{
		UpdateViewportCameraFocus();
	}

	if (IsUpdateRequestMadeFor(EUpdateFlags::ViewportSelection) && !IsUpdateRequestMadeFor(EUpdateFlags::ListSelection))
	{
		UpdateViewportSelection();
	}

	if (IsUpdateRequestMadeFor(EUpdateFlags::Highlight))
	{
		UpdateHighlights();
	}

	if (IsUpdateRequestMadeFor(EUpdateFlags::SizeMap))
	{
		UpdateSizeMap();
	}

	if (IsUpdateRequestMadeFor(EUpdateFlags::RebuildLevelList))
	{
		UpdateLevelList();
	}

	if (IsUpdateRequestMadeFor(EUpdateFlags::RebuildActorList))
	{
		UpdateActorList();
	}
}

void SLevelStreamAssistantWidget::UpdateLevelVisibility()
{
	if (CanControlLevelVisibility() && ShouldShowOnlyRelatedLevels())
	{
		bool bShowAll = !IsAnyActorSelected();

		// make bound levels visible
		for (ULevelStreaming* LevelStreaming : TWeakArraySafeRange<ULevelStreaming>(ListLevels))
		{
			bool bVisible = bShowAll || IsLevelBoundToAnySelectedActor(LevelStreamingEditor, LevelStreaming);

			LevelStreaming->SetShouldBeVisibleInEditor(bVisible);
		}

		LevelStreamAssistantUtils::FlushLevelStreaming();
	}
}

void SLevelStreamAssistantWidget::UpdateViewportCameraFocus()
{
	FBox FocusBox(ForceInit);

	if (ShouldFocusOnBoundLevels())
	{
		for (ULevelStreaming* StreamingLevel : TWeakArraySafeRange<ULevelStreaming>(ListLevels))
		{
			if (IsLevelBoundToAnySelectedActor(LevelStreamingEditor, StreamingLevel))
			{
				if (ULevel* Level = StreamingLevel->GetLoadedLevel())
				{
					FBox LevelBounds = ALevelBounds::CalculateLevelBounds(Level);
					if (LevelBounds.IsValid)
					{
						FocusBox += LevelBounds;
					}
				}
			}
		}
	}
	if (ShouldFocusOnSelectedActor())
	{
		for (AActor* SelectedActor : TWeakArraySafeRange<AActor>(LevelStreamingEditor->GetSelectedLevelStreamingActors()))
		{
			FocusBox += LevelStreamingEditor->GetLevelStreamingActorEditor().GetBounds(*SelectedActor);
		}
	}

	if (FocusBox.IsValid)
	{
		FocusBox = FocusBox.ExpandBy(500.f); // expand focus for more comfortable view.
		if (GEditor) GEditor->MoveViewportCamerasToBox(FocusBox, /*bActiveViewportOnly=*/false);
	}
}

void SLevelStreamAssistantWidget::UpdateViewportSelection()
{
	if (GEditor && ShouldSyncListSelectionWithViewportSelection())
	{
		GEditor->SelectNone(/*bNoteSelectionChange*/false, /*bDeselectBSPSurfs*/true);

		for (AActor* SelectedActor : TWeakArraySafeRange<AActor>(LevelStreamingEditor->GetSelectedLevelStreamingActors()))
		{
			GEditor->SelectActor(SelectedActor, /*bInSelected*/true, /*bNotify*/false);
		}

		GEditor->NoteSelectionChange();
	}
}

void SLevelStreamAssistantWidget::UpdateListSelection()
{
	if (!ShouldSyncListSelectionWithViewportSelection() || !TreeViewActors.IsValid() || !ActorHierarchy.IsValid() || !GEditor) return;

	TreeViewActors->ClearSelection(); // clear previous selection.

	TSharedPtr<FBaseItem> FirstSelection = nullptr;

	if (USelection* Selection = GEditor->GetSelectedActors())
	{
		TArray<UObject*> SelectedObjects;
		if (Selection->GetSelectedObjects(LevelStreamingEditor->GetLevelStreamingActorEditor().GetTargetType(), SelectedObjects) > 0)
		{
			// sync with first supported actor.
			for (UObject* SelectedObject : SelectedObjects)
			{
				if (AActor* SelectedActor = Cast<AActor>(SelectedObject))
				{
					if (LevelStreamingEditor->IsSupportedByEditor(SelectedActor))
					{
						// find selected actor item in the hierarchy
						TSharedPtr<FBaseItem> SelectedActorItem;
						FItemKey SelectedActorKey(SelectedActor);
						for (TSharedPtr<FBaseItem> Item : FItemHierarchyRange(ActorHierarchy->GetHierarchy()))
						{
							if (SelectedActorKey == FItemKey(*Item))
							{
								SelectedActorItem = Item;
								break;
							}
						}

						if (SelectedActorItem.IsValid())
						{
							TreeViewActors->SetItemSelection(SelectedActorItem, /*bSelected*/true);

							// first selected actor will be scrolled into view.
							if (!FirstSelection.IsValid()) FirstSelection = SelectedActorItem;
						}
					}
				}
			}
		}
	}

	// scroll first selected actor into view.
	if(FirstSelection.IsValid()) TreeViewActors->RequestScrollIntoView(FirstSelection);
}

void SLevelStreamAssistantWidget::UpdateHighlights()
{
	if (FLevelStreamAssistantEdMode* ActiveMode = LevelStreamAssistantUtils::GetPluginMode())
	{
		FSceneHighlights& SceneHighlights = ActiveMode->GetSceneHighlights();
		
		SceneHighlights.ClearHighlights();

		if (ShouldHighlightSelections())
		{
			// Highlight selected level streaming actors.
			for (AActor* SelectedActor : TWeakArraySafeRange<AActor>(LevelStreamingEditor->GetSelectedLevelStreamingActors()))
			{
				FLinearColor HighlightColor = LevelStreamingEditor->GetLevelStreamingActorEditor().SelectedActorColor;
				SceneHighlights.HighlightActor(SelectedActor, HighlightColor);
			}
		}

		bool bHoveredLevelIsHighlighted = false;

		// highlight hovered level
		if (ShouldHighlightSelections() && HoveredLevel.IsValid())
		{
			TOptional<FLinearColor> HighlightColor;
			TSharedPtr<FLevelStreamBindingMode> BindingMode;
			switch (GetBindingModeToSelectedActors(LevelStreamingEditor, HoveredLevel.Get(), BindingMode))
			{
			case EBindingModeResult::Success:
				HighlightColor = BindingMode->GetColor();
				break;

			case EBindingModeResult::MultipleBindings:
				HighlightColor = FLinearColor::Gray.CopyWithNewOpacity(.3f);
				break;

			case EBindingModeResult::NoBinding:
			default:
				// if binding is not set, still highlight with color of first available binding mode.
				if (TSharedPtr<FLevelStreamBindingMode> FirstBindingMode = GetFirstBindingMode(LevelStreamingEditor))
				{
					HighlightColor = FirstBindingMode->GetColor();
				}
				break;
			}

			if (HighlightColor.IsSet())
			{
				SceneHighlights.HighlightLevel(HoveredLevel, HighlightColor.GetValue(), true);
				bHoveredLevelIsHighlighted = true;
			}
		}

		// highlight related levels
		if (ShouldHighlightRelatedLevels())
		{
			for (ULevelStreaming* Level : TWeakArraySafeRange<ULevelStreaming>(ListLevels))
			{
				// skip if hovered level is already highlighted
				if (bHoveredLevelIsHighlighted && Level == HoveredLevel) continue;

				TOptional<FLinearColor> HighlightColor;
				TSharedPtr<FLevelStreamBindingMode> BindingMode;
				switch (GetBindingModeToSelectedActors(LevelStreamingEditor, Level, BindingMode))
				{
				case EBindingModeResult::Success:
					HighlightColor = BindingMode->GetColor();
					break;

				case EBindingModeResult::MultipleBindings:
					HighlightColor = FLinearColor::Gray.CopyWithNewOpacity(.2f);
					break;
				}

				if (HighlightColor.IsSet())
				{
					SceneHighlights.HighlightLevel(Level, HighlightColor.GetValue(), false);
				}
			}
		}
	}
}

void SLevelStreamAssistantWidget::UpdateSizeMap()
{
	if (ShouldCalculateSizeMap() && SizeMapButton.IsValid())
	{
		// Calculate disk and memory size of assets
		SizeMapButton->UpdateOverviewSize();
	}
}

void SLevelStreamAssistantWidget::UpdateLevelList()
{
	if (TreeViewLevels.IsValid())
	{
		TreeViewLevels->RebuildTree();
	}

	// forget about hovered level before list rebuid, otherwise unexpected hover event will set wrong visibility of levels.
	HoveredLevel.Reset();
}

void SLevelStreamAssistantWidget::UpdateActorList()
{
	if (TreeViewActors.IsValid())
	{
		TreeViewActors->RebuildTree();
	}
}

void SLevelStreamAssistantWidget::MakeUpdateRequest(EUpdateFlags Flags)
{
	UpdateFlags |= Flags;
}

void SLevelStreamAssistantWidget::RetractUpdateRequest(EUpdateFlags Flags)
{
	UpdateFlags &= ~Flags;
}

ULevelStreamAssistantWidgetSettings& SLevelStreamAssistantWidget::GetSettings() const
{
	ULevelStreamAssistantWidgetSettings* Settings = GetMutableDefault<ULevelStreamAssistantWidgetSettings>();
	check(Settings);
	return *Settings;
}

void SLevelStreamAssistantWidget::OnEditorTypePicked(UClass* Class)
{
	if (ensure(Class && Class->IsChildOf(ULevelStreamingActorEditorBase::StaticClass())))
	{
		LevelStreamingEditor->SetLevelStreamingActorEditorType(Class);

		ULevelStreamAssistantWidgetSettings& Settings = GetSettings();
		Settings.EditorType = Class;
		Settings.TargetType = LevelStreamingEditor->GetLevelStreamingActorEditor().GetTargetType();

		RefreshView();
	}
	if (ComboButton_EditorTypes.IsValid())
	{
		ComboButton_EditorTypes->SetIsOpen(false);
	}
}

void SLevelStreamAssistantWidget::OnTargetTypePicked(UClass* Class)
{
	if (ensure(Class && Class->IsChildOf(LevelStreamingEditor->GetLevelStreamingActorEditor().GetTargetType())))
	{
		GetSettings().TargetType = Class;
	}
	if (ComboButton_TargetTypes.IsValid())
	{
		ComboButton_TargetTypes->SetIsOpen(false);
	}
}

FText SLevelStreamAssistantWidget::GetEditorName() const
{
	UClass* EditorType = LevelStreamingEditor->GetLevelStreamingActorEditor().GetClass();
	return EditorType ? FText::FromName(EditorType->GetFName()) : FText::GetEmpty();
}

#undef LOCTEXT_NAMESPACE
