// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class AActor;
class ULevelStreaming;
class ULevel;

namespace LevelStreamAssistant
{
	struct FBaseItem;

	// key for folder or objects. used to quickly access tree items in the map when building tree hierarchy.
	struct FItemKey
	{
	private:
		TWeakObjectPtr<UObject> Object = nullptr;
		FName Folder = NAME_None;
		uint32 CachedHash = 0;
		bool bIsFolder = false;
		bool bIsLevel = false;
		bool bIsRoot = false;

	public:
		FItemKey(TWeakObjectPtr<ULevelStreaming> Level);
		FItemKey(TWeakObjectPtr<AActor> Actor);
		FItemKey(TWeakObjectPtr<UWorld> World);
		FItemKey(TWeakObjectPtr<ULevel> PersistentLevel);
		FItemKey(const FName& Folder, bool bIsLevel);
		FItemKey(const FBaseItem& Item);
		FItemKey() { }

		bool IsValid() const;
		bool IsFolder() const;
		bool IsLevel() const;
		bool IsRoot() const;

		TSharedPtr<FBaseItem> ConstructNewItem() const;

		friend bool operator==(const FItemKey& A, const FItemKey& B)
		{
			return	A.Folder	==	B.Folder	&&
					A.Object	==	B.Object	&&
					A.bIsFolder	==	B.bIsFolder	&&
					A.bIsLevel	==	B.bIsLevel	&&
					A.bIsRoot	==	B.bIsRoot;
		}

		friend uint32 GetTypeHash(const FItemKey& ItemID)
		{
			return ItemID.CachedHash;
		}

	private:
		void PreCalculateHash();
	};
}
