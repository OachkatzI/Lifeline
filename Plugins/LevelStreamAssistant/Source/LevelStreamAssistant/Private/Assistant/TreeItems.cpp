// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include <Assistant/TreeItems.h>
#include <Assistant/TreeItemVisitors.h>
#include <GameFramework/Actor.h>
#include <Engine/LevelStreaming.h>
#include <Engine/Level.h>
#include <Misc/Paths.h>
#include <Engine/World.h>
#include <Assistant/Folders/FolderManager.h>
#include <Utilities/LevelStreamAssistantUtils.h>

#define LOCTEXT_NAMESPACE "LevelStreamAssistant"

namespace LevelStreamAssistant
{
	FString FLevelItem::GetLabel() const
	{
		return Level.IsValid() ? FPaths::GetCleanFilename(Level->GetWorldAssetPackageName()) : TEXT("");
	}

	FText FLevelItem::GetToolTip() const
	{
		return Level.IsValid() ? FText::FromName(Level->GetWorldAssetPackageFName()) : FText::GetEmpty();
	}

	FString FActorItem::GetLabel() const
	{
		return Actor.IsValid() ? Actor->GetActorLabel() : TEXT("");
	}

	FText FActorItem::GetToolTip() const
	{
		return Actor.IsValid() ? FText::FromName(Actor->GetFName()) : FText::GetEmpty();
	}

	FString FActorFolderItem::GetLabel() const
	{
		return FPaths::GetPathLeaf(Path.ToString());
	}

	FText FActorFolderItem::GetToolTip() const
	{
		return FText::FromName(Path);
	}

	FString FLevelFolderItem::GetLabel() const
	{
		return FPaths::GetPathLeaf(Path.ToString());
	}

	FText FLevelFolderItem::GetToolTip() const
	{
		return FText::FromName(Path);
	}

	FString FActorRootItem::GetLabel() const
	{
		return World.IsValid() ? World->GetName() : TEXT("");
	}

	FText FActorRootItem::GetToolTip() const
	{
		return World.IsValid() ? FText::FromString(FPackageName::ObjectPathToPackageName(World->GetPathName())) : FText::GetEmpty();
	}

	FString FLevelRootItem::GetLabel() const
	{
		return PersistentLevel.IsValid() ? PersistentLevel->GetName() : TEXT("");
	}

	FText FLevelRootItem::GetToolTip() const
	{
		return PersistentLevel.IsValid() && PersistentLevel->OwningWorld ? FText::FromString(FPackageName::ObjectPathToPackageName(PersistentLevel->OwningWorld->GetPathName())) : FText::GetEmpty();
	}

	void FBaseItem::SetExpansion(bool InShouldExpandItem)
	{
		bIsExpanded = true;
	}

	bool FBaseItem::IsExpanded()
	{
		return bIsExpanded;
	}

	void FActorFolderItem::SetExpansion(bool InShouldExpandItem)
	{
		FBaseItem::SetExpansion(InShouldExpandItem);

		if (UWorld* World = LevelStreamAssistantUtils::GetWorldFromEditor())
		{
			FFolderManager& ActorFolderManager = FFolderManager::GetActorFolderManager();
			if (FFolderProperties* FolderProperties = ActorFolderManager.GetFolderProperties(*World, Path))
			{
				FolderProperties->bIsExpanded = InShouldExpandItem;
			}
		}
	}

	bool FActorFolderItem::IsExpanded()
	{
		if (UWorld* World = LevelStreamAssistantUtils::GetWorldFromEditor())
		{
			FFolderManager& ActorFolderManager = FFolderManager::GetActorFolderManager();
			if (const FFolderProperties* FolderProperties = ActorFolderManager.GetFolderProperties(*World, Path))
			{
				return FolderProperties->bIsExpanded;
			}
		}
		return FBaseItem::IsExpanded();
	}

	void FLevelFolderItem::SetExpansion(bool InShouldExpandItem)
	{
		FBaseItem::SetExpansion(InShouldExpandItem);

		if (UWorld* World = LevelStreamAssistantUtils::GetWorldFromEditor())
		{
			FFolderManager& LevelFolderManager = FFolderManager::GetLevelFolderManager();
			if (FFolderProperties* FolderProperties = LevelFolderManager.GetFolderProperties(*World, Path))
			{
				FolderProperties->bIsExpanded = InShouldExpandItem;
			}
		}
	}

	bool FLevelFolderItem::IsExpanded()
	{
		if (UWorld* World = LevelStreamAssistantUtils::GetWorldFromEditor())
		{
			FFolderManager& LevelFolderManager = FFolderManager::GetLevelFolderManager();
			if (const FFolderProperties* FolderProperties = LevelFolderManager.GetFolderProperties(*World, Path))
			{
				return FolderProperties->bIsExpanded;
			}
		}
		return FBaseItem::IsExpanded();
	}

	void FLevelItem::Accept(FItemVisitor& Visitor) const { Visitor.Visit(*this); }

	void FActorItem::Accept(FItemVisitor& Visitor) const { Visitor.Visit(*this); }

	void FActorFolderItem::Accept(FItemVisitor& Visitor) const { Visitor.Visit(*this); }
	
	void FLevelFolderItem::Accept(FItemVisitor& Visitor) const { Visitor.Visit(*this); }

	void FActorRootItem::Accept(FItemVisitor& Visitor) const { Visitor.Visit(*this); }

	void FLevelRootItem::Accept(FItemVisitor& Visitor) const { Visitor.Visit(*this); }
}

#undef LOCTEXT_NAMESPACE
