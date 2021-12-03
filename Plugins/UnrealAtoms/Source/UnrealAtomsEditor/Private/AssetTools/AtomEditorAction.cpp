// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AtomEditorAction.h"

#include "UnrealAtoms.h"
#include "UnrealAtoms/Public/Variables/AtomBaseVariable.h"
#include "Styling/SlateStyle.h"


// #define LOCTEXT_NAMESPACE "AssetTypeActions"
//
//
// FAtomEditorAction::FAtomEditorAction(const TSharedRef<ISlateStyle>& InStyle)
// 	: Style(InStyle)
// {
// }
//
// bool FAtomEditorAction::CanFilter()
// {
// 	return true;
// }
//
//
// uint32 FAtomEditorAction::GetCategories()
// {
// 	FUnrealAtomsModule& AtomsModule = FModuleManager::GetModuleChecked<FUnrealAtomsModule>("UnrealAtoms").Get();
// 	return AtomsModule.GetAtomsAssetCategoryBit();
// }
//
//
// FText FAtomEditorAction::GetName() const
// {
// 	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_UnrealAtoms", "Unreal Atoms");
// }
//
//
// UClass* FAtomEditorAction::GetSupportedClass() const
// {
// 	return UAtomBaseVariable::StaticClass();
// }
//
//
// FColor FAtomEditorAction::GetTypeColor() const
// {
// 	return FColor::Green;
// }
//
// #undef LOCTEXT_NAMESPACE
