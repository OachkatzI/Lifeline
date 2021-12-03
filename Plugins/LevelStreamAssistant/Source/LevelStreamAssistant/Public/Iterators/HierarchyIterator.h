// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// Simple depth first iterator for hierarchical data.
template<typename ItemType>
class THierarchyIterator
{
public:
    THierarchyIterator(const TArray<ItemType>& InHierarchy, TFunction<const TArray<ItemType>*(const ItemType&)> GetChildren, bool bEnd)
        : GetChildren(GetChildren)
    {
        if (!bEnd)
        {
            HierarchyStack.Push(&InHierarchy);
            Indices.Push(0);
            Advance();
        }
    }

    ItemType operator*() { return *Item; }

    // this comparison is only used to compare an iterator with ending iterator.
    bool operator==(const THierarchyIterator& Itr) { return Indices.Num() == Itr.Indices.Num(); }
    bool operator!=(const THierarchyIterator& Itr) { return !(*this == Itr); }

    THierarchyIterator& operator++()
    {
        Advance();
        return *this;
    }

private:

    void Advance()
    {
        while (HierarchyStack.Num() > 0)
        {
            int32& CurrentIndex = Indices.Last();
            const TArray<ItemType>* CurrentHierarchy = HierarchyStack.Last();
            check(CurrentHierarchy);

            if (CurrentIndex < CurrentHierarchy->Num())
            {
                Item = &CurrentHierarchy->GetData()[CurrentIndex];

                if (const TArray<ItemType>* Children = GetChildren(*Item))
                {
                    HierarchyStack.Push(Children);
                    Indices.Push(0);
                }

                CurrentIndex++;

                break;
            }
            else
            {
                HierarchyStack.Pop();
                Indices.Pop();
            }
        }
    }

    TArray<const TArray<ItemType>*> HierarchyStack;
    TArray<int32> Indices;
    TFunction<const TArray<ItemType>* (const ItemType&)> GetChildren;
    const ItemType* Item = nullptr;
};

// Simple range with depth first traversal for hierarchical data.
template<typename ItemType>
class THierarchyRange
{
public:
    THierarchyRange(const TArray<ItemType>& InHierarchy, TFunction<const TArray<ItemType>*(const ItemType&)> InGetChildren) 
        : Hierarchy(InHierarchy), GetChildren(InGetChildren) { }

    THierarchyIterator<ItemType> begin() { return THierarchyIterator<ItemType>(Hierarchy, GetChildren, false); }
    THierarchyIterator<ItemType> end() { return THierarchyIterator<ItemType>(Hierarchy, nullptr, true); }

private:
    const TArray<ItemType>& Hierarchy;
    TFunction<const TArray<ItemType>*(const ItemType&)> GetChildren;
};
