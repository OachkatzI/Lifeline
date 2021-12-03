// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Assistant/ItemContextMenuModifier.h"
#include <Assistant/TreeItemVisitors.h>
#include <Assistant/TreeItems.h>

namespace LevelStreamAssistant
{
	FItemContextMenuModifier::FItemContextMenuModifier()
		: bSupportsMultipleSelection(false)
		, bAppliesToActor(false)
		, bAppliesToActorFolder(false)
		, bAppliesToActorRoot(false)
		, bAppliesToLevel(false)
		, bAppliesToLevelFolder(false)
		, bAppliesToLevelRoot(false)
	{
	}

	FItemContextMenuModifier& FItemContextMenuModifier::OnModifyMenu(const FModifyContextMenu& InOnModifyMenu)
	{
		_OnModifyMenu = InOnModifyMenu;
		return *this;
	}

	FItemContextMenuModifier& FItemContextMenuModifier::SupportsMultipleSelection()
	{
		bSupportsMultipleSelection = true;
		return *this;
	}

	FItemContextMenuModifier& FItemContextMenuModifier::AppliesToActor()
	{
		bAppliesToActor = true;
		return *this;
	}

	FItemContextMenuModifier& FItemContextMenuModifier::AppliesToActorFolder()
	{
		bAppliesToActorFolder = true;
		return *this;
	}

	FItemContextMenuModifier& FItemContextMenuModifier::AppliesToActorRoot()
	{
		bAppliesToActorRoot = true;
		return *this;
	}

	FItemContextMenuModifier& FItemContextMenuModifier::AppliesToLevel()
	{
		bAppliesToLevel = true;
		return *this;
	}

	FItemContextMenuModifier& FItemContextMenuModifier::AppliesToLevelFolder()
	{
		bAppliesToLevelFolder = true;
		return *this;
	}

	FItemContextMenuModifier& FItemContextMenuModifier::AppliesToLevelRoot()
	{
		bAppliesToLevelRoot = true;
		return *this;
	}

	void FItemContextMenuModifier::ModifyMenu(FExtender& InMenuExtender) const
	{
		if (ensure(_OnModifyMenu))
		{
			_OnModifyMenu(InMenuExtender);
		}
	}

	bool FItemContextMenuModifier::DoesSupportMultipleSelection() const
	{
		return bSupportsMultipleSelection;
	}

	bool FItemContextMenuModifier::DoesApplyToItem(const FBaseItem& InItem) const
	{
		bool bAppliesToItem = false;
		InItem.Accept(FFunctionalItemVisitor()
			.Actor		([&](const FActorItem&)			{ bAppliesToItem = bAppliesToActor; })
			.ActorFolder([&](const FActorFolderItem&)	{ bAppliesToItem = bAppliesToActorFolder; })
			.ActorRoot	([&](const FActorRootItem&)		{ bAppliesToItem = bAppliesToActorRoot; })
			.Level		([&](const FLevelItem&)			{ bAppliesToItem = bAppliesToLevel; })
			.LevelFolder([&](const FLevelFolderItem&)	{ bAppliesToItem = bAppliesToLevelFolder; })
			.LevelRoot	([&](const FLevelRootItem&)		{ bAppliesToItem = bAppliesToLevelRoot; })
		);
		return bAppliesToItem;
	}
}
