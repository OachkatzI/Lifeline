// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Assistant/HierarchyBuilder.h"
#include <Assistant/TreeItems.h>
#include <Assistant/ItemKey.h>
#include <Utilities/LevelStreamAssistantUtils.h>
#include <Utilities/LevelStreamAssistantItemUtils.h>

namespace LevelStreamAssistant
{
	FHierarchyBuilder::FHierarchyBuilder(const FItemPassesFilter& InOnItemPassesFilter)
		: OnItemPassesFilter(InOnItemPassesFilter)
	{
	}

	const TArray<TSharedPtr<FBaseItem>>& FHierarchyBuilder::GetRootItems()
	{
		return RootItems;
	}

	TArray<TSharedPtr<FBaseItem>> FHierarchyBuilder::GetUsedItems()
	{
		TArray<TSharedPtr<FBaseItem>> UsedItems;
		IncludedItems.GenerateValueArray(UsedItems);
		return UsedItems;
	}

	TArray<TSharedPtr<FBaseItem>> FHierarchyBuilder::GetUnusedItems()
	{
		TArray<TSharedPtr<FBaseItem>> UnusedItems;
		ExcludedItems.GenerateValueArray(UnusedItems);
		return UnusedItems;
	}

	void FHierarchyBuilder::BuildHierarchy(const FItemKey& Key)
	{
		BuildHierarchyRecursive(Key, GetItem(Key));
	}

	void FHierarchyBuilder::ReuseHierarchy(const TArray<TSharedPtr<FBaseItem>>& InHierarchy)
	{
		for (TSharedPtr<FBaseItem> Item : InHierarchy)
		{
			if (Item.IsValid())
			{
				ReuseHierarchy(Item->Children);
				Item->Children.Empty();
				FItemKey Key(*Item);
				if (Key.IsValid()) ItemMap.Add(Key, Item);
			}
		}
	}

	void FHierarchyBuilder::BuildHierarchyRecursive(const FItemKey& ItemKey, TSharedPtr<FBaseItem> Item)
	{
		if (Item.IsValid())
		{
			if (IsItemIncluded(ItemKey)) return; // the hierarchy is already built for this item.
		
			FItemKey ParentKey = GetParentKey(Item);
			if (TSharedPtr<FBaseItem> ParentItem = GetItem(ParentKey)) // if item has parent
			{
				if (FilterItem(ItemKey, Item))
				{
					ParentItem->Children.Add(Item);
					Item->Parent = ParentItem;
				}

				BuildHierarchyRecursive(ParentKey, ParentItem);
			}
			else // this is a root item
			{
				if (FilterItem(ItemKey, Item))
				{
					RootItems.Add(Item);
				}
			}
		}
	}

	bool FHierarchyBuilder::FilterItem(const FItemKey& Key, TSharedPtr<FBaseItem> Item)
	{
		// include item in hierarchy if it passes the filter.
		// items that are excluded may be included later but included items don't get excluded later.
		if (ItemPassesFilter(Item))
		{
			ExcludedItems.Remove(Key); // remove from excluded items
			IncludedItems.Add(Key, Item); // include the item
			return true;
		}
		else
		{
			ExcludedItems.Add(Key, Item);
			return false;
		}
	}

	bool FHierarchyBuilder::IsItemIncluded(const FItemKey& Key)
	{
		return IncludedItems.Contains(Key);
	}

	bool FHierarchyBuilder::ItemPassesFilter(TSharedPtr<FBaseItem> Item)
	{
		// if item has any children that passed the filter, it must be included in the tree.
		return (Item.IsValid() && Item->Children.Num() > 0) || !OnItemPassesFilter.IsBound() || OnItemPassesFilter.Execute(Item);
	}

	FItemKey FHierarchyBuilder::GetParentKey(TSharedPtr<FBaseItem> Item)
	{
		FItemKey ParentKey;
		if (ensure(Item.IsValid()))
		{
			FItemKey ItemKey = FItemKey(*Item);
			
			if (!ItemKey.IsRoot())
			{
				FName ParentFolder = LevelStreamAssistantUtils::GetItemFolderPath(*Item);
				if (ParentFolder == NAME_None)
				{
					if (UWorld* Root = LevelStreamAssistantUtils::GetWorldFromEditor())
					{
						if (ItemKey.IsLevel())
						{
							ParentKey = FItemKey(Root->PersistentLevel);
						}
						else
						{
							ParentKey = FItemKey(Root);
						}
					}
				}
				else
				{
					ParentKey = FItemKey(ParentFolder, ItemKey.IsLevel());
				}
			}
		}
		return ParentKey;
	}

	TSharedPtr<FBaseItem> FHierarchyBuilder::GetItem(const FItemKey& Key)
	{
		if (TSharedPtr<FBaseItem> Item = FindItem(Key)) return Item;
		else return NewItem(Key);
	}

	TSharedPtr<FBaseItem> FHierarchyBuilder::FindItem(const FItemKey& Key)
	{
		if (TSharedPtr<FBaseItem>* ItemPtr = ItemMap.Find(Key)) return *ItemPtr;
		return nullptr;
	}

	TSharedPtr<FBaseItem> FHierarchyBuilder::NewItem(const FItemKey& Key)
	{
		TSharedPtr<FBaseItem> Item = Key.ConstructNewItem();
		if (Item.IsValid())
		{
			ItemMap.Add(Key, Item);
		}
		return Item;
	}
}
