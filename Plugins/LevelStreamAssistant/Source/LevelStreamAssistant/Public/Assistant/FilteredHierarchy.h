// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Assistant/ItemPassesFilter.h>

namespace LevelStreamAssistant
{
	struct FBaseItem;
	struct FItemKey;

	// represents a hierarchy of items that can be temporarily filtered.
	class FFilteredHierarchy
	{
	public:
		void ResetItems(const TArray<FItemKey>& ItemKeys, const FItemPassesFilter& InOnItemPassesFilter);
		void FilterHierarchy(const FItemPassesFilter& InOnItemPassesFilter);
		
		template<typename TComparer>
		void Sort(const TComparer& Comparer)
		{
			Sort(Hierarchy, Comparer);
		}

		const TArray<TSharedPtr<LevelStreamAssistant::FBaseItem>>& GetHierarchy() { return Hierarchy; }

	private:
		template<typename TComparer>
		void Sort(TArray<TSharedPtr<LevelStreamAssistant::FBaseItem>>& InHierarchy, const TComparer& Comparer)
		{
			InHierarchy.StableSort(Comparer);
			for (TSharedPtr<LevelStreamAssistant::FBaseItem> SubItem : InHierarchy)
			{
				if(SubItem.IsValid()) Sort(SubItem->Children, Comparer);
			}
		}

		TArray<TSharedPtr<LevelStreamAssistant::FBaseItem>> Hierarchy;
		TArray<TSharedPtr<LevelStreamAssistant::FBaseItem>> AllItems;
	};
}