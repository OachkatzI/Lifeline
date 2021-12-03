// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// Simple iterator for array of TWeakObjectPtr. this will only iterate over valid pointers.
template<typename ItemType>
class TWeakArraySafeIterator
{
public:
    TWeakArraySafeIterator(const TArray<TWeakObjectPtr<ItemType>>& InArray, bool bEnd) : Array(InArray)
    {
        CurrentIndex = bEnd ? InArray.Num() : INDEX_NONE;
        Advance(); // find first valid pointer.
    }

    // returns a valid pointer.
    ItemType* operator*() { return Entry; }

    bool operator==(const TWeakArraySafeIterator& Itr) { return CurrentIndex == Itr.CurrentIndex; }
    bool operator!=(const TWeakArraySafeIterator& Itr) { return !(*this == Itr); }

    // find next valid reference or end iterator if there are no remaining valid references.
    TWeakArraySafeIterator& operator++()
    {
        Advance();
        return *this;
    }

private:

    // find next valid pointer.
    void Advance()
    {
        check(CurrentIndex >= INDEX_NONE);

        Entry = nullptr;
        do
        {
            CurrentIndex++;
            if (CurrentIndex < Array.Num())
            {
                Entry = Array[CurrentIndex].Get();
            }
            else
            {
                CurrentIndex = Array.Num();
                break;
            }
        } while (!Entry);
    }

    const TArray<TWeakObjectPtr<ItemType>>& Array;
    int32 CurrentIndex;
    ItemType* Entry;
};

// Simple range for array of TWeakObjectPtr. this will only iterate over valid pointers.
template<typename ItemType>
class TWeakArraySafeRange
{
public:
    TWeakArraySafeRange(const TArray<TWeakObjectPtr<ItemType>>& InArray) : Array(InArray) { }

    TWeakArraySafeIterator<ItemType> begin() { return TWeakArraySafeIterator<ItemType>(Array, false); }
    TWeakArraySafeIterator<ItemType> end() { return TWeakArraySafeIterator<ItemType>(Array, true); }

private:
    const TArray<TWeakObjectPtr<ItemType>>& Array;
};
