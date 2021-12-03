// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include <Assistant/FilteredHierarchy.h>
#include <Assistant/ItemKey.h>
#include <Assistant/HierarchyBuilder.h>

namespace LevelStreamAssistant
{
	void FFilteredHierarchy::FilterHierarchy(const FItemPassesFilter& InOnItemPassesFilter)
	{
		FHierarchyBuilder Builder(InOnItemPassesFilter);

		// rebuild hierarchy with existing items.
		Builder.ReuseHierarchy(AllItems);

		for (TSharedPtr<FBaseItem> Item : AllItems)
		{
			Builder.BuildHierarchy(FItemKey(*Item));
		}

		Hierarchy = Builder.GetRootItems();
	}

	void FFilteredHierarchy::ResetItems(const TArray<FItemKey>& ItemKeys, const FItemPassesFilter& InOnItemPassesFilter)
	{
		FHierarchyBuilder Builder(InOnItemPassesFilter);

		// add items to builder so they might be reused.
		Builder.ReuseHierarchy(AllItems);

		for (const FItemKey& ItemKey : ItemKeys)
		{
			Builder.BuildHierarchy(ItemKey);
		}

		Hierarchy = Builder.GetRootItems();

		// get all the items that were either filtered or not so they can be reused later.
		AllItems = Builder.GetUsedItems();
		AllItems.Append(Builder.GetUnusedItems());
	}
}
