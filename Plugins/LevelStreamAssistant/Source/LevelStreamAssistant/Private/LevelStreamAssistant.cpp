// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "LevelStreamAssistant.h"
#include "EditorMode/LevelStreamAssistantEdMode.h"
#include "EditorMode/LevelStreamAssistantSettings.h"
#include "LevelStreamAssistantStyle.h"
#include <LevelStreamAssistantCommands.h>
#include "EditorMode/LevelStreamAssistantSettingsCustomization.h"
#include "Assistant/SLevelStreamAssistantWidget.h"
#include <Assistant/Folders/FolderManager.h>
#include <SelectionTool/ActorSelectorBoxCustomization.h>
#include <SelectionTool/ActorSelectorBox.h>
#include "Widgets/Docking/SDockTab.h"
#include "LevelEditor.h"
#include "WorkspaceMenuStructure/Public/WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"

IMPLEMENT_MODULE(FLevelStreamAssistantModule, LevelStreamAssistant);

static const FName LevelStreamAssistantTabId("LevelStreamAssistant");

#define LOCTEXT_NAMESPACE "FLevelStreamAssistantModule"

void FLevelStreamAssistantModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FLevelStreamAssistantStyle::Initialize();
	FLevelStreamAssistantStyle::ReloadTextures();

	FLevelStreamAssistantCommands::Register();

	LevelStreamAssistant::FFolderManager::Initialize();

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditorModule.RegisterCustomClassLayout(ULevelStreamAssistantSettings::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FLevelStreamAssistantSettingsCustomization::MakeInstance));
	PropertyEditorModule.RegisterCustomClassLayout(AActorSelectorBox::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FActorSelectorBoxCustomization::MakeInstance));

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	RegisterLevelStreamAssistantTab(LevelEditorModule.GetLevelEditorTabManager()); // register to an already existing tab manager.
	LevelEditorModule.OnTabManagerChanged().Remove(LevelStreamAssistantTabHandle); // remove previous handle
	LevelStreamAssistantTabHandle = LevelEditorModule.OnTabManagerChanged().AddLambda([]()
	{
		FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
		RegisterLevelStreamAssistantTab(LevelEditorModule.GetLevelEditorTabManager()); // register to a new tab manager.
	});

	FEditorModeRegistry::Get().RegisterMode<FLevelStreamAssistantEdMode>(
		FLevelStreamAssistantEdMode::EM_LevelStreamAssistantEdModeId,
		LOCTEXT("LevelStreamAssistantEdModeName", "Level Stream Assistant"),
		FSlateIcon(FLevelStreamAssistantStyle::GetStyleSetName(), "LSA.Logo.Large", "LSA.Logo.Medium"),
		true);
}

void FLevelStreamAssistantModule::ShutdownModule()
{	
	// This function may be called during shutdown to clean up your module. For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
	FEditorModeRegistry::Get().UnregisterMode(FLevelStreamAssistantEdMode::EM_LevelStreamAssistantEdModeId);

	if (FModuleManager::Get().IsModuleLoaded("LevelEditor"))
	{
		FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
		LevelEditorModule.OnTabManagerChanged().Remove(LevelStreamAssistantTabHandle);

		if (TSharedPtr<FTabManager> TabManager = LevelEditorModule.GetLevelEditorTabManager())
		{
			TabManager->UnregisterTabSpawner(LevelStreamAssistantTabId);
		}
	}

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyEditorModule.UnregisterCustomClassLayout(ULevelStreamAssistantSettings::StaticClass()->GetFName());
		PropertyEditorModule.UnregisterCustomClassLayout(AActorSelectorBox::StaticClass()->GetFName());
	}

	LevelStreamAssistant::FFolderManager::Shutdown();

	FLevelStreamAssistantCommands::Unregister();

	FLevelStreamAssistantStyle::Shutdown();
}

void FLevelStreamAssistantModule::PreUnloadCallback()
{
	// force close level stream assistant tab before plugin module is shutdown and the style becomes invalid.
	if (FSlateApplicationBase::IsInitialized() && FModuleManager::Get().IsModuleLoaded("LevelEditor"))
	{
		FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");

		if (TSharedPtr<FTabManager> TabManager = LevelEditorModule.GetLevelEditorTabManager())
		{
			if (TabManager->HasTabSpawner(LevelStreamAssistantTabId))
			{
				if (TSharedPtr<SDockTab> LevelStreamAssistantTab = TabManager->TryInvokeTab(LevelStreamAssistantTabId))
				{
					LevelStreamAssistantTab->RequestCloseTab();
				}
			}
		}
	}
}

void FLevelStreamAssistantModule::SummonLevelStreamAssistantTab()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	if (TSharedPtr<FTabManager> TabManager = LevelEditorModule.GetLevelEditorTabManager())
	{
		if (TabManager->HasTabSpawner(LevelStreamAssistantTabId))
		{
			TabManager->TryInvokeTab(LevelStreamAssistantTabId);
		}
	}
}

TSharedRef<SDockTab> FLevelStreamAssistantModule::OnSpawnLevelStreamAssistantTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab) // TODO: when tab is not focused remove highlights
	[
		SNew(SLevelStreamAssistantWidget)
	];
}

void FLevelStreamAssistantModule::RegisterLevelStreamAssistantTab(TSharedPtr<class FTabManager> TabManager)
{
	if (TabManager.IsValid() && !TabManager->HasTabSpawner(LevelStreamAssistantTabId))
	{
		TabManager->RegisterTabSpawner(LevelStreamAssistantTabId,
			FOnSpawnTab::CreateStatic(&FLevelStreamAssistantModule::OnSpawnLevelStreamAssistantTab))
			.SetDisplayName(LOCTEXT("LevelStreamAssistantTabTitle", "Level Stream Assistant"))
			.SetIcon(FSlateIcon(FLevelStreamAssistantStyle::GetStyleSetName(), "LSA.Logo.Small"))
			.SetGroup(WorkspaceMenu::GetMenuStructure().GetLevelEditorCategory());
	}
}

#undef LOCTEXT_NAMESPACE
