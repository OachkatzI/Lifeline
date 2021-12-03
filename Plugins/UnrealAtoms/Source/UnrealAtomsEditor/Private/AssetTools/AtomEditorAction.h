// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AssetTypeActions_Base.h"
#include "UnrealAtoms.h"
#include "Templates/SharedPointer.h"


class ISlateStyle;

/**
 * Implements an action for AtomVariables assets.
 */
template <typename T>
class FAtomEditorAction
	: public FAssetTypeActions_Base
{
public:

	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InStyle The style set to use for asset editor toolkits.
	 * @param AtomDisplayLiteral The name that will appear in the context menu when you create the atom
	 * @param SubMenu The submenu that this atom will be organised under
	 * @param AtomDisplayColor The Color that the Atom will appear as in the editor
	 */
	FAtomEditorAction(const TSharedRef<ISlateStyle>& InStyle, const FText AtomDisplayLiteral, const FText SubMenu, FColor AtomDisplayColor)
		: Style(InStyle),
		  DisplayLiteral(AtomDisplayLiteral),
		  SubMenuLiteral(SubMenu),
		  DisplayColor(AtomDisplayColor)
	{
	}


	virtual bool CanFilter() override
	{
		return true;
	}


	virtual uint32 GetCategories() override
	{
		FUnrealAtomsModule& AtomsModule = FModuleManager::GetModuleChecked<FUnrealAtomsModule>("UnrealAtoms").Get();
		return AtomsModule.GetAtomsAssetCategoryBit();
	}


	virtual const TArray<FText>& GetSubMenus() const override
	{
		static const TArray<FText> SubMenus
		{
			SubMenuLiteral
		};
		return SubMenus;
	}


	virtual FText GetName() const override
	{
		return DisplayLiteral;
	}


	virtual UClass* GetSupportedClass() const override
	{
		return T::StaticClass();
	}


	virtual FColor GetTypeColor() const override
	{
		return DisplayColor;
	}


private:
	/** Pointer to the style set to use for toolkits. */
	TSharedRef<ISlateStyle> Style;
	FText                   DisplayLiteral;
	FText                   SubMenuLiteral;
	FColor                  DisplayColor;
};

// class ISlateStyle;
//
// /**
//  * Implements an action for AtomVariables assets.
//  */
// class FAtomEditorAction
// 	: public FAssetTypeActions_Base
// {
// public:
//
// 	/**
// 	 * Creates and initializes a new instance.
// 	 *
// 	 * @param InStyle The style set to use for asset editor toolkits.
// 	 */
// 	FAtomEditorAction(const TSharedRef<ISlateStyle>& InStyle);
//
// 	virtual bool    CanFilter() override;
// 	virtual uint32  GetCategories() override;
// 	virtual FText   GetName() const override;
// 	virtual UClass* GetSupportedClass() const override;
// 	virtual FColor  GetTypeColor() const override;
//
// private:
// 	/** Pointer to the style set to use for toolkits. */
// 	TSharedRef<ISlateStyle> Style;
// };
