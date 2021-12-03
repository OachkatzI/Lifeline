// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include <EditorUndoClient.h>

class ULevelStreamAssistantWidgetSettings;
class ULevelStreaming;
class AActor;
template<typename ItemType> class SSimpleSearchBox;
class SSizeMapButton;
class STextBlock;
class SComboButton;
class FReply;
class FTransactionObjectEvent;
namespace EColumnSortPriority { enum Type; }
namespace EColumnSortMode { enum Type; }
struct FSlateBrush;
typedef FName FEditorModeID;
struct FAssetData;
class FUICommandList;
class SWindow;
class ULevelStreamingActorEditorBase;

namespace LevelStreamAssistant 
{ 
	struct FBaseItem;
	class STreeViewActors;
	class STreeViewLevels;
	class FFilteredHierarchy;
	class ILevelStreamingEditor;
}

/**
* This is the widget that manage level streaming volumes and streaming levels.
*/
class SLevelStreamAssistantWidget : public SCompoundWidget, public FEditorUndoClient
{
public:
	SLATE_BEGIN_ARGS(SLevelStreamAssistantWidget)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	SLevelStreamAssistantWidget();
	~SLevelStreamAssistantWidget();

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	// flags used to update various parts of the view.
	enum class EUpdateFlags : uint16
	{
		None = 0 << 0,
		LevelVisibility = 1 << 0,
		ViewportFocus = 1 << 1,
		ViewportSelection = 1 << 2,
		ListSelection = 1 << 3,
		Highlight = 1 << 4,
		SizeMap = 1 << 5,
		RebuildActorList = 1 << 6,
		RebuildLevelList = 1 << 7,
		PopulateLists = 1 << 8,

		RebuildLists = RebuildActorList | RebuildLevelList,
		LevelBinding = LevelVisibility | ViewportFocus | SizeMap | Highlight | RebuildLevelList,
	};

private:
	void BindCommands();

	TSharedRef<SWidget> BuildOptionsToolbar() const;

	void OnSearchChanged_ActorsTree();
	void OnSearchChanged_LevelsTree();

	void OnHovered_Level(TWeakObjectPtr<ULevelStreaming> InLevel);
	void OnUnhovered_Level(TWeakObjectPtr<ULevelStreaming> InLevel);

	void OnLevelHovered(TSharedPtr<LevelStreamAssistant::FBaseItem> LevelItem);
	void OnLevelUnhovered(TSharedPtr<LevelStreamAssistant::FBaseItem> LevelItem);

	bool CanToggleLevelVisibility() const;

	void OnLevelBindingChanged();
	void OnSelectedActorsChanged();

	FReply OnClicked_MakeNewFolder_Actors() const;
	bool CanMakeNewFolder_Actors() const;

	FReply OnClicked_MakeNewFolder_Levels() const;
	bool CanMakeNewFolder_Levels() const;

	TSharedRef<SWidget> OnGetMenuContent_EditorType();
	TSharedRef<SWidget> OnGetMenuContent_TargetType();

	TSubclassOf<AActor> GetActorTypeForDragAndDrop() const;
	const FSlateBrush* GetIconForActorDragAndDrop() const;

	bool GetIsOverviewSizeDisplayed() const;
	TArray<FAssetData> GetAssetDataList() const;

	void ExecuteAction_ShowOnlyRelatedLevels();
	void ExecuteAction_HighlightRelatedLevels();
	void ExecuteAction_FocusOnBoundLevels();
	void ExecuteAction_FocusOnSelectedActor();
	void ExecuteAction_SyncListSelectionWithViewportSelection();
	void ExecuteAction_PreviewLevelVisibility();
	void ExecuteAction_HighlightSelections();
	void ExecuteAction_CalculateSizeMap();

	void OpenEditorDetailsPanel();
	void OnEditorDetailsPanelClosed(const TSharedRef<SWindow>& Window, TWeakObjectPtr<ULevelStreamingActorEditorBase> InCustomizedEditor);
	void OnFinishedChangingEditorProperties(const FPropertyChangedEvent& PropertyChangedEvent);

	bool CanExecuteAction_PreviewLevelVisibility() const;

	bool ShouldShowOnlyRelatedLevels() const;
	bool ShouldHighlightRelatedLevels() const;
	bool ShouldFocusOnBoundLevels() const;
	bool ShouldFocusOnSelectedActor() const;
	bool ShouldSyncListSelectionWithViewportSelection() const;
	bool ShouldPreviewLevelVisibility() const;
	bool ShouldHighlightSelections() const;
	bool ShouldCalculateSizeMap() const;

	bool CanControlLevelVisibility() const;

	void OnObjectTransacted(UObject* Object, const FTransactionObjectEvent& TransactionObjectEvent);
	void OnEditorSelectionChanged(UObject* NewSelection);
	void OnRefreshLevelBrowser();
	void OnMapOpened(const FString& MapName, bool bAsTemplate);
	void OnEditorModeChanged(const FEditorModeID& ModeId);
	void OnFolderRenamed(const FName& OldName, const FName& NewName);
	void OnObjectFolderChanged(const UObject* InActor, const FName& OldPath, const FName& NewPath);
	void OnObjectsReplaced(const TMap<UObject*, UObject*>& ReplacementMap);
	void OnActorMoving(AActor* MovingActor);

	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;

	bool IsAnyActorSelected() const;

	void OnColumnSortModeChanged(const EColumnSortPriority::Type SortPriority, const FName& ColumnId, const EColumnSortMode::Type InSortMode);

	void SortLevels();
	void SortActors();

	void FixWarning_AutomaticLevelStreaming();
	bool ShouldBeFixed_AutomaticLevelStreaming() const;

	void FixWarning_ViewportRealtimeRendering();
	bool ShouldBeFixed_ViewportRealtimeRendering() const;

	void RefreshView();
	void PopulateLists();
	void PopulateActorTree();
	void PopulateLevelTree();
	void UpdateViewport();

	void UpdateLevelVisibility();
	void UpdateViewportCameraFocus();
	void UpdateViewportSelection();
	void UpdateListSelection();
	void UpdateHighlights();
	void UpdateSizeMap();
	void UpdateLevelList();
	void UpdateActorList();

	// update the view on next tick.
	FORCEINLINE void MakeUpdateRequest(EUpdateFlags Flags);
	FORCEINLINE void RetractUpdateRequest(EUpdateFlags Flags);
	FORCEINLINE void ClearUpdateRequest() { UpdateFlags = EUpdateFlags::None; }
	FORCEINLINE bool IsUpdateRequestMade() const { return UpdateFlags != EUpdateFlags::None; }
	FORCEINLINE bool IsUpdateRequestMadeFor(EUpdateFlags Flags) const { return EnumHasAllFlags(UpdateFlags, Flags); }

	ULevelStreamAssistantWidgetSettings& GetSettings() const;
	void OnEditorTypePicked(UClass* Class);
	void OnTargetTypePicked(UClass* Class);
	FText GetEditorName() const;

	TSharedPtr<LevelStreamAssistant::ILevelStreamingEditor> LevelStreamingEditor;

	TArray<TWeakObjectPtr<ULevelStreaming>> ListLevels;
	TArray<TWeakObjectPtr<AActor>> ListActors;

	TSharedPtr<LevelStreamAssistant::FFilteredHierarchy> ActorHierarchy;
	TSharedPtr<LevelStreamAssistant::FFilteredHierarchy> LevelHierarchy;

	TSharedPtr<LevelStreamAssistant::STreeViewActors> TreeViewActors;
	TSharedPtr<LevelStreamAssistant::STreeViewLevels> TreeViewLevels;

	TSharedPtr<SSimpleSearchBox<TWeakObjectPtr<ULevelStreaming>>> SearchBoxLevels;
	TSharedPtr<SSimpleSearchBox<TWeakObjectPtr<AActor>>> SearchBoxActors;

	TSharedPtr<SSimpleSearchBox<TSharedPtr<LevelStreamAssistant::FBaseItem>>> SearchBoxActorItems;
	TSharedPtr<SSimpleSearchBox<TSharedPtr<LevelStreamAssistant::FBaseItem>>> SearchBoxLevelItems;

	TSharedPtr<SComboButton> ComboButton_EditorTypes;
	TSharedPtr<SComboButton> ComboButton_TargetTypes;
	TSharedPtr<SComboButton> ComboButton_Options;

	TSharedPtr<SSizeMapButton> SizeMapButton;

	TSharedPtr<STextBlock> TextBlock_DiskSize;
	TSharedPtr<STextBlock> TextBlock_MemorySize;

	TSharedPtr<FUICommandList> CommandList;

	TWeakPtr<SWindow> OpenedEditorDetailsPanelWeakPtr;

	TWeakObjectPtr<ULevelStreaming> HoveredLevel;

	// how the editor should be updated on next tick
	EUpdateFlags UpdateFlags = EUpdateFlags::None;

	uint8 bHoveredLevelWasVisible : 1;

	uint8 bSortLevelsAscending : 1;
	uint8 bSortActorsAscending : 1;
	
	const static FName ActorColumnId;
	const static FName LevelColumnId;
};

ENUM_CLASS_FLAGS(SLevelStreamAssistantWidget::EUpdateFlags)
