// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Widgets/SCompoundWidget.h>
#include <Widgets/DeclarativeSyntaxSupport.h>

class FReply;
struct FSlateBrush;

namespace LevelStreamAssistant
{
	struct FBaseItem;
	struct FLevelItem;
	struct FLevelRootItem;

	// toggle visibility of FBaseItem (only for Folders and Levels)
	class SToggleVisibility : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SToggleVisibility) {}
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, TSharedRef<FBaseItem> InItem, const TArray<TSharedPtr<FBaseItem>>* InHierarchy);

	private:
		const TArray<TSharedPtr<FBaseItem>>* Hierarchy = nullptr;
		TSharedPtr<FBaseItem> Item;

		FReply OnClicked_ToggleVisibilityButton();

		const FSlateBrush* GetLevelVisibilityBrush() const;

		static bool IsAnyAssociatedLevelVisible(const FBaseItem& Item);

		static void SetVisibilityToAssociatedLevel(const FLevelItem& LevelItem, bool bShouldBeVisible);
		static void SetVisibilityToAssociatedLevel(const FLevelRootItem& Root, bool bShouldBeVisible);
		
		static bool ItemIsRoot(const FBaseItem& Item);
	};
}
