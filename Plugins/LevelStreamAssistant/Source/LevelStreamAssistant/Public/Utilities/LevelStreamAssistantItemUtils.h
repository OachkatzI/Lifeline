// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FLevelStreamAssistant_FolderProperties;

namespace LevelStreamAssistant
{
	class FFolderManager;
	struct FBaseItem;
	typedef FLevelStreamAssistant_FolderProperties FFolderProperties;
}

/**
 * set of helper functions used across level stream assistant plugin.
 */
namespace LevelStreamAssistantUtils
{
	using namespace LevelStreamAssistant;

	// @return true if ItemA should come before ItemB in a hierarchy.
	bool CompareItems(TSharedPtr<FBaseItem> ItemA, TSharedPtr<FBaseItem> ItemB, bool bAscending);

	// @return actor folder properties of an item.
	FFolderProperties* GetItemFolderProperties(const FBaseItem& Item);

	// @return item path in hierarchy.
	FName GetItemFolderPath(const FBaseItem& Item);

	// set item path in hierarchy.
	void SetItemFolderPath(const FBaseItem& Item, const FName& NewPath);

	// @return item path of items in hierarchy.
	TArray<FName> GetFolderPathOfItems(TArray<TSharedPtr<FBaseItem>> Items);

	// create a new folder for items in the hierarchy. returns name of the created folder.
	FName CreateFolder(FFolderManager& InFolderManager, TArray<TSharedPtr<FBaseItem>> Items);

	// move items in hierarchy.
	void MoveItems(TArray<TSharedPtr<FBaseItem>> Items, const FName& NewPath);

	// rename item in hierarchy.
	void RenameItem(const FBaseItem& Item, const FName& NewName);

	// delete item in hierarchy.
	void DeleteItem(const FBaseItem& Item);

	// used for filtering items when user types in search box.
	void ItemToStringArray(TSharedPtr<FBaseItem> Item, TArray<FString>& Array);
};
