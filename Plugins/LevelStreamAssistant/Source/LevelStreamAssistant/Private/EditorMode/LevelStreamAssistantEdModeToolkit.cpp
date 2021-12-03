// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "EditorMode/LevelStreamAssistantEdModeToolkit.h"
#include "EditorMode/LevelStreamAssistantEdMode.h"
#include "LevelStreamAssistant.h"
#include "EditorMode/LevelStreamAssistantSettings.h"
#include "LevelStreamAssistantStyle.h"
#include "Modules/ModuleManager.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include <Widgets/SDragDropActor.h>
#include "EditorModeManager.h"
#include <SelectionTool/ActorSelectorBox.h>

#define LOCTEXT_NAMESPACE "FLevelStreamAssistantEdModeToolkit"

FLevelStreamAssistantEdModeToolkit::FLevelStreamAssistantEdModeToolkit()
{
}

void FLevelStreamAssistantEdModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{
	FPropertyEditorModule& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea);
	DetailsViewArgs.bShowPropertyMatrixButton = false;
	DetailsView = PropertyEditor.CreateDetailView(DetailsViewArgs);

	FLevelStreamAssistantEdMode* EdMode = GetLevelStreamingEditorMode();
	if (DetailsView.IsValid() && EdMode)
	{
		DetailsView->SetObject(&EdMode->GetSettings());

		FToolBarBuilder ToolButtons(NULL, FMultiBoxCustomization::None);
		ToolButtons.AddToolBarButton(
			FUIAction(FExecuteAction::CreateSP(this, &FLevelStreamAssistantEdModeToolkit::OnClicked_ToggleLevelColoration)), 
			NAME_None, 
			LOCTEXT("LabelLevelColoration","Level Coloration"),
			LOCTEXT("ToolTipLevelColoration", "Toggle Level Coloration"),
			FSlateIcon(FLevelStreamAssistantStyle::GetStyleSetName(), "LSA.Tools.LevelColoration")
		);
		ToolButtons.AddToolBarButton(
			FUIAction(FExecuteAction::CreateSP(this, &FLevelStreamAssistantEdModeToolkit::OnClicked_ShowLevelStreamAssistantTab)),
			NAME_None,
			LOCTEXT("LabelLevelStreamAssistant", "Level Stream Assistant"),
			LOCTEXT("ToolTipLevelStreamAssistant", "Show Level Stream Assistant Tab"),
			FSlateIcon(FLevelStreamAssistantStyle::GetStyleSetName(), "LSA.Tools.LevelStreamAssistantTab")
		);

		TSharedRef<SWidget> ActorSelectorVolumeDragAndDropButton = 
			SNew(SDragDropActor)
			.ActorType(AActorSelectorBox::StaticClass())
			.Image(FLevelStreamAssistantStyle::Get().GetBrush("LSA.Tools.ActorSelectorBox"))
			.ToolTipText(LOCTEXT("ToolTipDragAndDropActorSelectorBox", "Drag and drop Actor Selector Box."));
		ToolButtons.AddToolBarWidget(ActorSelectorVolumeDragAndDropButton, LOCTEXT("LabelDragAndDropActorSelectorBox", "Box Selection"));

		// todo: add button to color code streaming volumes with different usage.

		SAssignNew(ToolkitWidget, SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		[
			ToolButtons.MakeWidget()
		]
		+ SVerticalBox::Slot()
		[
			DetailsView.ToSharedRef()
		];
	}

	FModeToolkit::Init(InitToolkitHost);
}

FName FLevelStreamAssistantEdModeToolkit::GetToolkitFName() const
{
	return FName("Level Stream Assistant Tool");
}

FText FLevelStreamAssistantEdModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "Level Stream Assistant Tool");
}

class FEdMode* FLevelStreamAssistantEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FLevelStreamAssistantEdMode::EM_LevelStreamAssistantEdModeId);
}

FLevelStreamAssistantEdMode* FLevelStreamAssistantEdModeToolkit::GetLevelStreamingEditorMode() const
{
	return GLevelEditorModeTools().GetActiveModeTyped<FLevelStreamAssistantEdMode>(FLevelStreamAssistantEdMode::EM_LevelStreamAssistantEdModeId);
}

void FLevelStreamAssistantEdModeToolkit::OnClicked_ToggleLevelColoration()
{
	if (GEditor)
	{
		if (FViewport* Viewport = GEditor->GetActiveViewport())
		{
			if (FViewportClient* Client = Viewport->GetClient())
			{
				if (FEngineShowFlags* ShowFlags = Client->GetEngineShowFlags())
				{
					ShowFlags->SetLevelColoration(!ShowFlags->LevelColoration);
				}
			}
		}
	}
}

void FLevelStreamAssistantEdModeToolkit::OnClicked_ShowLevelStreamAssistantTab()
{
	FLevelStreamAssistantModule& Module = FModuleManager::GetModuleChecked<FLevelStreamAssistantModule>("LevelStreamAssistant");
	Module.SummonLevelStreamAssistantTab();
}

#undef LOCTEXT_NAMESPACE
