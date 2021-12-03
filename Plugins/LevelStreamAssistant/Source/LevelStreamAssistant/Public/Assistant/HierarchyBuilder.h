// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Assistant/ItemPassesFilter.h>
#include <Assistant/ItemKey.h>

namespace LevelStreamAssistant
{
	struct FBaseItem;

	// bottom up hierarchy builder for actors, levels and folders.
	struct FHierarchyBuilder
	{
		FHierarchyBuilder(const FItemPassesFilter& InOnItemPassesFilter);
		
		// build hierarchy for the item and it's parents. new items are created if they are missing.
		void BuildHierarchy(const FItemKey& Key);

		// add all items of hierarchy for builder to reuse.
		void ReuseHierarchy(const TArray<TSharedPtr<FBaseItem>>& InHierarchy);

		const TArray<TSharedPtr<FBaseItem>>& GetRootItems();
		TArray<TSharedPtr<FBaseItem>> GetUsedItems();
		TArray<TSharedPtr<FBaseItem>> GetUnusedItems();
	
	private:
		void BuildHierarchyRecursive(const FItemKey& ItemKey, TSharedPtr<FBaseItem> Item);

		// put the item either in included or excluded list.
		bool FilterItem(const FItemKey& ItemKey, TSharedPtr<FBaseItem> Item);

		// returns true if item is included in the hierarchy.
		bool IsItemIncluded(const FItemKey& ItemKey);

		bool ItemPassesFilter(TSharedPtr<FBaseItem> Item);

		// get key for the the parent node. (for example attached parent actor or parent folder).
		FItemKey GetParentKey(TSharedPtr<FBaseItem> Item);

		// find or create a new item using a key.
		TSharedPtr<FBaseItem> GetItem(const FItemKey& ItemKey);

		// find an item by key.
		TSharedPtr<FBaseItem> FindItem(const FItemKey& ItemKey);

		// create a new item using a key.
		TSharedPtr<FBaseItem> NewItem(const FItemKey& ItemKey);

	private:
		// used to quickly find items anywhere in the hierarchy.
		TMap<FItemKey, TSharedPtr<FBaseItem>> ItemMap;

		// items included in hierarchy.
		TMap<FItemKey, TSharedPtr<FBaseItem>> IncludedItems;

		// items excluded from hierarchy and didn't pass the filter.
		TMap<FItemKey, TSharedPtr<FBaseItem>> ExcludedItems;

		// root nodes of the hierarchy.
		TArray<TSharedPtr<FBaseItem>> RootItems;

		// used to filter items in the hierarchy.
		FItemPassesFilter OnItemPassesFilter;
	};
}