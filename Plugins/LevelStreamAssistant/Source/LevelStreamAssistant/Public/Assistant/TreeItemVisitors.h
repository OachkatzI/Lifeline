// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace LevelStreamAssistant
{
	struct FLevelItem;
	struct FActorItem;
	struct FActorFolderItem;
	struct FLevelFolderItem;
	struct FActorRootItem;
	struct FLevelRootItem;

	// visitor pattern implementation for tree items.
	struct FItemVisitor
	{
		virtual ~FItemVisitor() { }
		virtual void Visit(const FLevelItem& Item) = 0;
		virtual void Visit(const FActorItem& Item) = 0;
		virtual void Visit(const FActorFolderItem& Item) = 0;
		virtual void Visit(const FLevelFolderItem& Item) = 0;
		virtual void Visit(const FActorRootItem& Item) = 0;
		virtual void Visit(const FLevelRootItem& Item) = 0;
	};

	// provides ability to implement various behaviours for tree items without creating custom visitor objects.
	struct FFunctionalItemVisitor : public FItemVisitor
	{
		typedef TFunctionRef<void(const FLevelItem&)>		FLevelFunction;
		typedef TFunctionRef<void(const FActorItem&)>		FActorFunction;
		typedef TFunctionRef<void(const FActorFolderItem&)>	FActorFolderFunction;
		typedef TFunctionRef<void(const FLevelFolderItem&)>	FLevelFolderFunction;
		typedef TFunctionRef<void(const FActorRootItem&)>	FActorRootFunction;
		typedef TFunctionRef<void(const FLevelRootItem&)>	FLevelRootFunction;

		FFunctionalItemVisitor& Level(FLevelFunction InFunction) { LevelFunction = InFunction; return *this; }
		FFunctionalItemVisitor& Actor(FActorFunction InFunction) { ActorFunction = InFunction; return *this; }
		FFunctionalItemVisitor& ActorFolder(FActorFolderFunction InFunction) { ActorFolderFunction = InFunction; return *this; }
		FFunctionalItemVisitor& LevelFolder(FLevelFolderFunction InFunction) { LevelFolderFunction = InFunction; return *this; }
		FFunctionalItemVisitor& ActorRoot(FActorRootFunction InFunction) { ActorRootFunction = InFunction; return *this; }
		FFunctionalItemVisitor& LevelRoot(FLevelRootFunction InFunction) { LevelRootFunction = InFunction; return *this; }

		virtual ~FFunctionalItemVisitor() override { }

	private:
		TOptional<FLevelFunction> LevelFunction;
		TOptional<FActorFunction> ActorFunction;
		TOptional<FActorFolderFunction> ActorFolderFunction;
		TOptional<FLevelFolderFunction> LevelFolderFunction;
		TOptional<FActorRootFunction> ActorRootFunction;
		TOptional<FLevelRootFunction> LevelRootFunction;

		virtual void Visit(const FLevelItem& Item) override { if (LevelFunction.IsSet()) { LevelFunction.GetValue()(Item); } }
		virtual void Visit(const FActorItem& Item) override { if (ActorFunction.IsSet()) { ActorFunction.GetValue()(Item); } }
		virtual void Visit(const FActorFolderItem& Item) override { if (ActorFolderFunction.IsSet()) { ActorFolderFunction.GetValue()(Item); } }
		virtual void Visit(const FLevelFolderItem& Item) override { if (LevelFolderFunction.IsSet()) { LevelFolderFunction.GetValue()(Item); } }
		virtual void Visit(const FActorRootItem& Item) override { if (ActorRootFunction.IsSet()) { ActorRootFunction.GetValue()(Item); } }
		virtual void Visit(const FLevelRootItem& Item) override { if (LevelRootFunction.IsSet()) { LevelRootFunction.GetValue()(Item); } }
	};
}
