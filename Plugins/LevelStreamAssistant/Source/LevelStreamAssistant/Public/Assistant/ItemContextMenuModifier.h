// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FExtender;

namespace LevelStreamAssistant
{
	struct FBaseItem;

	// used to modify context menus conditionally based on the selected items.
	class FItemContextMenuModifier : public TSharedFromThis<FItemContextMenuModifier>
	{
	public:
		typedef TFunction<void(FExtender&)> FModifyContextMenu;

		FItemContextMenuModifier();

		static FItemContextMenuModifier& New() { return *(new FItemContextMenuModifier()); }

		FItemContextMenuModifier& OnModifyMenu(const FModifyContextMenu& InOnModifyMenu);

		FItemContextMenuModifier& SupportsMultipleSelection();

		FItemContextMenuModifier& AppliesToActor();
		FItemContextMenuModifier& AppliesToActorFolder();
		FItemContextMenuModifier& AppliesToActorRoot();

		FItemContextMenuModifier& AppliesToLevel();
		FItemContextMenuModifier& AppliesToLevelFolder();
		FItemContextMenuModifier& AppliesToLevelRoot();
		
		void ModifyMenu(FExtender& InMenuExtender) const;

		bool DoesSupportMultipleSelection() const;

		bool DoesApplyToItem(const FBaseItem& InItem) const;

	private:
		FModifyContextMenu _OnModifyMenu;

		uint8 bSupportsMultipleSelection : 1;

		uint8 bAppliesToActor : 1;
		uint8 bAppliesToActorFolder : 1;
		uint8 bAppliesToActorRoot : 1;
		uint8 bAppliesToLevel : 1;
		uint8 bAppliesToLevelFolder : 1;
		uint8 bAppliesToLevelRoot : 1;
	};
}