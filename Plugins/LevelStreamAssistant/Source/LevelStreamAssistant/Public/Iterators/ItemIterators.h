// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <Assistant/TreeItems.h>
#include <Iterators/HierarchyIterator.h>

namespace LevelStreamAssistant
{
    class FItemHierarchyRange
    {
    public:
        FItemHierarchyRange(const TArray<TSharedPtr<FBaseItem>>& InHierarchy) : Hierarchy(InHierarchy) { }

        THierarchyIterator<TSharedPtr<FBaseItem>> begin() { return THierarchyIterator<TSharedPtr<FBaseItem>>(Hierarchy, &FItemHierarchyRange::GetChildren, false); }
        THierarchyIterator<TSharedPtr<FBaseItem>> end() { return THierarchyIterator<TSharedPtr<FBaseItem>>(Hierarchy, nullptr, true); }

    private:
        const TArray<TSharedPtr<FBaseItem>>& Hierarchy;

        static const TArray<TSharedPtr<FBaseItem>>* GetChildren(const TSharedPtr<FBaseItem>& Item)
        {
            return Item.IsValid() ? &Item->Children : nullptr;
        }
    };
}
