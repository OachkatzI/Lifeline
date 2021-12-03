// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealAtoms.h"

#if WITH_EDITOR
#include "Developer/AssetTools/Public/IAssetTools.h"
#include "Developer/AssetTools/Public/AssetToolsModule.h"
#endif


#define LOCTEXT_NAMESPACE "FUnrealAtomsModule"


void FUnrealAtomsModule::StartupModule()
{
#if WITH_EDITOR
	FModuleManager::LoadModulePtr<IModuleInterface>("UnrealAtoms");

	if (GIsEditor)
	{
		// Register asset types
		auto& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

		// register Atoms category so that Atoms assets can register to it
		AtomsAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("UAtom")), LOCTEXT("UAtom", "Unreal Atoms"));
	}
#endif
}


void FUnrealAtomsModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUnrealAtomsModule, UnrealAtoms)
