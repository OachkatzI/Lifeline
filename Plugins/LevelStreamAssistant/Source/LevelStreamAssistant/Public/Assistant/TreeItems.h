// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class AActor;
class ULevelStreaming;
class ULevel;

namespace LevelStreamAssistant
{
	struct FItemVisitor;

	// tree item for level stream assistant trees.
	struct FBaseItem : public TSharedFromThis<FBaseItem>
	{
		TArray<TSharedPtr<FBaseItem>> Children;
		TWeakPtr<FBaseItem> Parent;

		FBaseItem() { }

		virtual ~FBaseItem() { }

		virtual FString GetLabel() const = 0;
		virtual FText GetToolTip() const = 0;

		virtual void SetExpansion(bool InShouldExpandItem);
		virtual bool IsExpanded();

		virtual void Accept(FItemVisitor& Visitor) const = 0;
		
		FSimpleDelegate OnRequestRename;

	private:
		bool bIsExpanded = true;
	};

	// tree item representing a level.
	struct FLevelItem : public FBaseItem
	{
		TWeakObjectPtr<ULevelStreaming> Level;
		FLevelItem(TWeakObjectPtr<ULevelStreaming> InLevel) : Level(InLevel) { }
		virtual ~FLevelItem() override { }

		virtual FString GetLabel() const override;
		virtual FText GetToolTip() const override;

		virtual void Accept(FItemVisitor& Visitor) const override;
	};

	// tree item representing an actor.
	struct FActorItem : public FBaseItem
	{
		TWeakObjectPtr<AActor> Actor;
		FActorItem(TWeakObjectPtr<AActor> InActor) : Actor(InActor) { }
		virtual ~FActorItem() override { }

		virtual FString GetLabel() const override;
		virtual FText GetToolTip() const override;

		virtual void Accept(FItemVisitor& Visitor) const override;
	};

	// tree item representing a folder for actors.
	struct FActorFolderItem : public FBaseItem
	{
		FName Path;
		FActorFolderItem(const FName& InPath) : Path(InPath) { }
		virtual ~FActorFolderItem() override { }

		virtual FString GetLabel() const override;
		virtual FText GetToolTip() const override;

		virtual void SetExpansion(bool InShouldExpandItem) override;
		virtual bool IsExpanded() override;

		virtual void Accept(FItemVisitor& Visitor) const override;
	};

	// tree item representing a folder for levels.
	struct FLevelFolderItem: public FBaseItem
	{
		FName Path;
		FLevelFolderItem(const FName& InPath) : Path(InPath) { }
		virtual ~FLevelFolderItem() override { }

		virtual FString GetLabel() const override;
		virtual FText GetToolTip() const override;

		virtual void SetExpansion(bool InShouldExpandItem) override;
		virtual bool IsExpanded() override;

		virtual void Accept(FItemVisitor& Visitor) const override;
	};

	// tree root item representing world of actors.
	struct FActorRootItem : public FBaseItem
	{
		TWeakObjectPtr<UWorld> World;
		FActorRootItem(TWeakObjectPtr<UWorld> InWorld) : World(InWorld) { }
		virtual ~FActorRootItem() override { }

		virtual FString GetLabel() const override;
		virtual FText GetToolTip() const override;

		virtual void Accept(FItemVisitor& Visitor) const override;
	};

	// tree root item representing world of levels.
	struct FLevelRootItem : public FBaseItem
	{
		TWeakObjectPtr<ULevel> PersistentLevel;
		FLevelRootItem(TWeakObjectPtr<ULevel> InPersistentLevel) : PersistentLevel(InPersistentLevel) { }
		virtual ~FLevelRootItem() override { }

		virtual FString GetLabel() const override;
		virtual FText GetToolTip() const override;

		virtual void Accept(FItemVisitor& Visitor) const override;
	};
}
