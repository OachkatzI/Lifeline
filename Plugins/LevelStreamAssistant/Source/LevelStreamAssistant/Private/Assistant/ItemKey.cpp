// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include <Assistant/ItemKey.h>
#include <Assistant/TreeItems.h>
#include <Assistant/TreeItemVisitors.h>

namespace LevelStreamAssistant
{
	FItemKey::FItemKey(TWeakObjectPtr<ULevelStreaming> Level)
		: Object(Level), bIsFolder(false), bIsLevel(true), bIsRoot(false)
	{
		PreCalculateHash();
	}

	FItemKey::FItemKey(TWeakObjectPtr<AActor> Actor)
		: Object(Actor), bIsFolder(false), bIsLevel(false), bIsRoot(false)
	{
		PreCalculateHash();
	}

	FItemKey::FItemKey(TWeakObjectPtr<UWorld> World)
		: Object(World), bIsFolder(false), bIsLevel(false), bIsRoot(true)
	{
		PreCalculateHash();
	}

	FItemKey::FItemKey(TWeakObjectPtr<ULevel> PersistentLevel)
		: Object(PersistentLevel), bIsFolder(false), bIsLevel(true), bIsRoot(true)
	{
		PreCalculateHash();
	}

	FItemKey::FItemKey(const FName& Folder, bool bIsLevel) 
		: Folder(Folder), bIsFolder(true), bIsLevel(bIsLevel), bIsRoot(false)
	{
		PreCalculateHash();
	}

	FItemKey::FItemKey(const FBaseItem& Item)
	{
		Item.Accept(FFunctionalItemVisitor()
			.Actor([this](const FActorItem& ActorItem) { Object = ActorItem.Actor; bIsFolder = false; bIsLevel = false; bIsRoot = false; })
			.Level([this](const FLevelItem& LevelItem) { Object = LevelItem.Level; bIsFolder = false; bIsLevel = true; bIsRoot = false; })
			.ActorFolder([this](const FActorFolderItem& FolderItem) { Folder = FolderItem.Path; bIsFolder = true; bIsLevel = false; bIsRoot = false; })
			.LevelFolder([this](const FLevelFolderItem& FolderItem) { Folder = FolderItem.Path; bIsFolder = true; bIsLevel = true; bIsRoot = false; })
			.ActorRoot([this](const FActorRootItem& RootItem) { Object = RootItem.World; bIsFolder = false; bIsLevel = false; bIsRoot = true; })
			.LevelRoot([this](const FLevelRootItem& RootItem) { Object = RootItem.PersistentLevel; bIsFolder = false; bIsLevel = true; bIsRoot = true; })
		);

		PreCalculateHash();
	}

	bool FItemKey::IsValid() const
	{
		return (IsFolder() && Folder != NAME_None) || (!IsFolder() && Object.IsValid());
	}

	bool FItemKey::IsFolder() const
	{
		return bIsFolder;
	}

	bool FItemKey::IsLevel() const
	{
		return bIsLevel;
	}

	bool FItemKey::IsRoot() const
	{
		return bIsRoot;
	}

	TSharedPtr<FBaseItem> FItemKey::ConstructNewItem() const
	{
		TSharedPtr<FBaseItem> Item;
		if (IsValid())
		{
			if (IsFolder())
			{
				if (IsLevel())
				{
					Item = MakeShareable(new FLevelFolderItem(Folder));
				}
				else
				{
					Item = MakeShareable(new FActorFolderItem(Folder));
				}
			}
			else if (AActor* Actor = Cast<AActor>(Object.Get()))
			{
				Item = MakeShareable(new FActorItem(Actor));
			}
			else if (ULevelStreaming* Level = Cast<ULevelStreaming>(Object.Get()))
			{
				Item = MakeShareable(new FLevelItem(Level));
			}
			else if (UWorld* World = Cast<UWorld>(Object.Get()))
			{
				Item = MakeShareable(new FActorRootItem(World));
			}
			else if (ULevel* PersistentLevel = Cast<ULevel>(Object.Get()))
			{
				Item = MakeShareable(new FLevelRootItem(PersistentLevel));
			}
		}
		return Item;
	}
	
	void FItemKey::PreCalculateHash()
	{
		CachedHash = 0;
		if (IsFolder())
		{
			if (Folder != NAME_None) CachedHash = GetTypeHash(Folder);
		}
		else
		{
			if (Object.IsValid()) CachedHash = GetTypeHash(Object);
		}

		if (IsLevel()) CachedHash = HashCombine(CachedHash, 1500450271); 
		if (IsRoot()) CachedHash = HashCombine(999999937, CachedHash);
	}
}
