// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
* Level Stream Assistant plugin module.
*/
class FLevelStreamAssistantModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual void PreUnloadCallback() override;
	//virtual void PostLoadCallback() override;

	LEVELSTREAMASSISTANT_API void SummonLevelStreamAssistantTab();

private:
	static TSharedRef<class SDockTab> OnSpawnLevelStreamAssistantTab(const class FSpawnTabArgs& SpawnTabArgs);

	static void RegisterLevelStreamAssistantTab(TSharedPtr<class FTabManager> TabManager);

	FDelegateHandle LevelStreamAssistantTabHandle;
};
