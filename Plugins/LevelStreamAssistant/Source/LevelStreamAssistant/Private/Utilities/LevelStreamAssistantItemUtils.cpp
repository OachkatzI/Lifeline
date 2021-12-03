// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Utilities/LevelStreamAssistantItemUtils.h"
#include <Utilities/LevelStreamAssistantUtils.h>
#include <Misc/Paths.h>
#include <GameFramework/Actor.h>
#include <Engine/LevelStreaming.h>
#include <Assistant/TreeItems.h>
#include <Assistant/TreeItemVisitors.h>
#include <Assistant/Folders/FolderManager.h>
#include <EditorLevelUtils.h>
#include <Editor.h>

#define LOCTEXT_NAMESPACE "LevelStreamAssistantItemUtils"

namespace LevelStreamAssistantUtils
{
	bool CompareItems(TSharedPtr<FBaseItem> ItemA, TSharedPtr<FBaseItem> ItemB, bool bAscending)
	{
		if (!ItemA.IsValid() && !ItemB.IsValid()) return true;
		if (!ItemA.IsValid()) return false;
		if (!ItemB.IsValid()) return true;
		
		bool ItemAComesFirst = true;

		ItemA->Accept(FFunctionalItemVisitor()
			.Actor([&](const FActorItem& ActorItem)
			{
				ItemB->Accept(FFunctionalItemVisitor()
					.Actor([&](const FActorItem& ActorItem2) { ItemAComesFirst = CompareActorNames(ActorItem.Actor, ActorItem2.Actor, bAscending); })
					.ActorFolder([&](const FActorFolderItem& FolderItem2) { ItemAComesFirst = false; }) // folders come first
				);
			})
			.Level([&](const FLevelItem& LevelItem)
			{
				ItemB->Accept(FFunctionalItemVisitor()
					.Level([&](const FLevelItem& LevelItem2) { ItemAComesFirst = CompareLevelNames(LevelItem.Level, LevelItem2.Level, bAscending); })
					.LevelFolder([&](const FLevelFolderItem& FolderItem2) { ItemAComesFirst = false; }) // folders come first
				);
			})
			.ActorFolder([&](const FActorFolderItem& FolderItem)
			{
				ItemB->Accept(FFunctionalItemVisitor()
					.Actor([&](const FActorItem& ActorItem2) { ItemAComesFirst = true; })
					.ActorFolder([&](const FActorFolderItem& FolderItem2) { ItemAComesFirst = CompareNames(FolderItem.Path.ToString(), FolderItem2.Path.ToString(), bAscending); })
				);
			})
			.LevelFolder([&](const FLevelFolderItem& FolderItem)
			{
				ItemB->Accept(FFunctionalItemVisitor()
					.Level([&](const FLevelItem& LevelItem2) { ItemAComesFirst = true; })
					.LevelFolder([&](const FLevelFolderItem& FolderItem2) { ItemAComesFirst = CompareNames(FolderItem.Path.ToString(), FolderItem2.Path.ToString(), bAscending); })
				);
			})
		);

		return ItemAComesFirst;
	}
	
	FFolderProperties* GetItemFolderProperties(const FBaseItem& Item)
	{
		FFolderProperties* FolderProperties = nullptr;

		if (UWorld* World = LevelStreamAssistantUtils::GetWorldFromEditor())
		{
			Item.Accept(FFunctionalItemVisitor()
				.ActorFolder([&FolderProperties, World](const FActorFolderItem& FolderItem)
				{
					FolderProperties =
						FFolderManager::GetActorFolderManager()
							.GetFolderProperties(*World, FolderItem.Path);
				})
				.LevelFolder([&FolderProperties, World](const FLevelFolderItem& FolderItem)
				{
					FolderProperties =
						FFolderManager::GetLevelFolderManager()
							.GetFolderProperties(*World, FolderItem.Path);
				})
			);
		}

		return FolderProperties;
	}

	FName GetItemFolderPath(const FBaseItem& Item)
	{
		FName Path = NAME_None;
		Item.Accept(FFunctionalItemVisitor()
			.Actor([&Path](const FActorItem& ActorItem)
			{
				if (UWorld* World = GetWorldFromEditor())
				{
					Path = FFolderManager::GetActorFolderManager().GetFolderPathOf(*World, ActorItem.Actor);
				}
			})
			.Level([&Path](const FLevelItem& LevelItem)
			{
				if (UWorld* World = GetWorldFromEditor())
				{
					ULevelStreaming* LevelStreaming = LevelItem.Level.Get();
					ULevel* Level = LevelStreaming ? LevelStreaming->GetLoadedLevel() : nullptr;
					
					Path = FFolderManager::GetLevelFolderManager().GetFolderPathOf(*World, Level);
				}
			})
			.ActorFolder([&Path](const FActorFolderItem& FolderItem)
			{
				Path = *FPaths::GetPath(FolderItem.Path.ToString());
			})
			.LevelFolder([&Path](const FLevelFolderItem& FolderItem)
			{
				Path = *FPaths::GetPath(FolderItem.Path.ToString());
			})
		);
		return Path;
	}

	void SetItemFolderPath(const FBaseItem& Item, const FName& NewPath)
	{
		Item.Accept(FFunctionalItemVisitor()
			.Actor([&NewPath](const FActorItem& ActorItem)
			{
				if(UWorld* World = GetWorldFromEditor())
				{
					FFolderManager::GetActorFolderManager().SetFolderPathTo(*World, ActorItem.Actor, NewPath);
				}
			})
			.Level([&NewPath](const FLevelItem& LevelItem)
			{
				if (UWorld* World = GetWorldFromEditor())
				{
					ULevelStreaming* LevelStreaming = LevelItem.Level.Get();
					ULevel* Level = LevelStreaming ? LevelStreaming->GetLoadedLevel() : nullptr;
					FFolderManager::GetLevelFolderManager().SetFolderPathTo(*World, Level, NewPath);
				}
			})
			.ActorFolder([&NewPath](const FActorFolderItem& FolderItem)
			{
				if (UWorld* World = GetWorldFromEditor())
				{
					FString FolderName = FPaths::GetPathLeaf(FolderItem.Path.ToString());
					FString BasePath = NewPath.ToString();
					FName MovedFolderPath = *(NewPath.IsNone() ? FolderName : (BasePath / FolderName));

					FFolderManager::GetActorFolderManager().RenameFolder(*World, FolderItem.Path, MovedFolderPath);

					// note: although folder path is changed, but FolderItem is not modified, this problem is resolved by rebuilding the hierarchy with new items.
				}
			})
			.LevelFolder([&NewPath](const FLevelFolderItem& FolderItem)
			{
				if (UWorld* World = GetWorldFromEditor())
				{
					FString FolderName = FPaths::GetPathLeaf(FolderItem.Path.ToString());
					FString BasePath = NewPath.ToString();
					FName MovedFolderPath = *(NewPath.IsNone() ? FolderName : (BasePath / FolderName));

					FFolderManager::GetLevelFolderManager().RenameFolder(*World, FolderItem.Path, MovedFolderPath);

					// note: although folder path is changed, but FolderItem is not modified, this problem is resolved by rebuilding the hierarchy with new items.
				}
			})
		);
	}

	TArray<FName> GetFolderPathOfItems(TArray<TSharedPtr<FBaseItem>> Items)
	{
		TArray<FName> FolderPaths;
		FolderPaths.Reserve(Items.Num());
		for (TSharedPtr<FBaseItem> Item : Items)
		{
			if (Item.IsValid())
			{
				FolderPaths.Add(GetItemFolderPath(*Item));
			}
		}
		return FolderPaths;
	}

	FName CreateFolder(FFolderManager& InFolderManager, TArray<TSharedPtr<FBaseItem>> Items)
	{
		if (UWorld* World = GetWorldFromEditor())
		{
			TArray<FName> Paths = GetFolderPathOfItems(Items);
			FName CommonPath = GetCommonPath(Paths);
			FName NewFolderName = InFolderManager.GetUniqueFolderName(*World, CommonPath);

			FScopedTransaction Transaction(LOCTEXT("TransactionCreateFolder", "Create Folder"));
			InFolderManager.CreateFolder(*World, NewFolderName);
			MoveItems(Items, NewFolderName);
			return NewFolderName;
		}
		return NAME_None;
	}

	void MoveItems(TArray<TSharedPtr<FBaseItem>> Items, const FName& NewPath)
	{
		// child items should be moved first when moving multiple items in a hierarchy.
		// this is because moving an item will also move it's children, 
		// so invalid movement operation can be made when moving items in wrong order.
		Items.Sort([](const TSharedPtr<FBaseItem>& PossibleChild, const TSharedPtr<FBaseItem>& PossibleParent)
		{
			if (!PossibleChild.IsValid()) return false;
			if (!PossibleParent.IsValid()) return true;

			TWeakPtr<FBaseItem> ParentItem = PossibleChild->Parent;
			while (ParentItem.IsValid())
			{
				if (ParentItem.Pin() == PossibleParent)
				{
					return true; // child comes first
				}

				ParentItem = ParentItem.Pin()->Parent;
			}
			// PossibleChild is not the actual child but PossibleParent could still be the actual child so it should come first
			// if items are not in the same hierarchy the order doesn't matter.
			return false;
		});

		for (TSharedPtr<FBaseItem> Item : Items)
		{
			if (Item.IsValid())
			{
				SetItemFolderPath(*Item, NewPath);
			}
		}
	}

	void RenameItem(const FBaseItem& Item, const FName& NewName)
	{
		if (NewName.IsNone()) return;

		Item.Accept(FFunctionalItemVisitor()
			.Actor([&NewName](const FActorItem& ActorItem)
			{
				if(AActor* Actor = ActorItem.Actor.Get())
				{
					FActorLabelUtilities::RenameExistingActor(Actor, NewName.ToString());
				}
			})
			.ActorFolder([&NewName](const FActorFolderItem& FolderItem)
			{
				if (UWorld* World = LevelStreamAssistantUtils::GetWorldFromEditor())
				{
					FString BasePath = FPaths::GetPath(FolderItem.Path.ToString());
					FString NewPath = BasePath / NewName.ToString();
					FFolderManager::GetActorFolderManager().RenameFolder(*World, FolderItem.Path, *NewPath);
				}
			})
			.LevelFolder([&NewName](const FLevelFolderItem& FolderItem)
			{
				if (UWorld* World = LevelStreamAssistantUtils::GetWorldFromEditor())
				{
					FString BasePath = FPaths::GetPath(FolderItem.Path.ToString());
					FString NewPath = BasePath / NewName.ToString();
					FFolderManager::GetLevelFolderManager().RenameFolder(*World, FolderItem.Path, *NewPath);
				}
			})
		);
	}

	void DeleteItem(const FBaseItem& Item)
	{
		Item.Accept(FFunctionalItemVisitor()
			.Actor([](const FActorItem& ActorItem)
			{
				if (!ensure(GEditor)) return;
				if(AActor* ActorToDelete = ActorItem.Actor.Get())
				{
					GEditor->SelectNone(false, true);
					GEditor->SelectActor(ActorToDelete, true, true, true);
					GEditor->edactDeleteSelected(ActorToDelete->GetWorld());
				}
			})
			.ActorFolder([](const FActorFolderItem& FolderItem)
			{
				if (UWorld* World = LevelStreamAssistantUtils::GetWorldFromEditor())
				{
					FFolderManager::GetActorFolderManager().DeleteFolder(*World, FolderItem.Path);
				}
			})
			.Level([](const FLevelItem& LevelItem)
			{
				if (LevelItem.Level.IsValid())
				{
					ULevelStreaming* LevelStreaming = LevelItem.Level.Get();
					ULevel* Level = LevelStreaming ? LevelStreaming->GetLoadedLevel() : nullptr;
					if (Level)
					{
						EditorLevelUtils::RemoveLevelFromWorld(Level);
					}
				}
			})
			.LevelFolder([](const FLevelFolderItem& FolderItem)
			{
				if (UWorld* World = LevelStreamAssistantUtils::GetWorldFromEditor())
				{
					FFolderManager::GetLevelFolderManager().DeleteFolder(*World, FolderItem.Path);
				}
			})
		);
	}

	// used for filtering items when user types in search box.
	void ItemToStringArray(TSharedPtr<FBaseItem> Item, TArray<FString>& Array)
	{
		if (Item.IsValid())
		{
			Item->Accept(FFunctionalItemVisitor()
				.Actor([&Array](const FActorItem& Item)
				{
					if (Item.Actor.IsValid())
					{
						Array.Add(Item.Actor->GetActorLabel());
						//Array.Add(Item.Actor->GetPathName());
					}
				})
				.Level([&Array](const FLevelItem& Item)
				{
					if (Item.Level.IsValid())
					{
						Array.Add(FPaths::GetPathLeaf(Item.Level->GetWorldAssetPackageName()));
					}
				})
				.ActorFolder([&Array](const FActorFolderItem& Item)
				{
					if (!Item.Path.IsNone())
					{
						Array.Add(FPaths::GetPathLeaf(Item.Path.ToString()));
					}
				})
				.LevelFolder([&Array](const FLevelFolderItem& Item)
				{
					if (!Item.Path.IsNone())
					{
						Array.Add(FPaths::GetPathLeaf(Item.Path.ToString()));
					}
				})
				.ActorRoot([&Array](const FActorRootItem& Item)
				{
					if (Item.World.IsValid())
					{
						Array.Add(Item.World->GetName());
					}
				})
				.LevelRoot([&Array](const FLevelRootItem& Item)
				{
					if (Item.PersistentLevel.IsValid())
					{
						Array.Add(Item.PersistentLevel->GetName());
					}
				})
			);
		}
	}
}

#undef LOCTEXT_NAMESPACE
