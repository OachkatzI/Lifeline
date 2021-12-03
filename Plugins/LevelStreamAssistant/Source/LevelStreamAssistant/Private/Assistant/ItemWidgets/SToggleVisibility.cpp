// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include <Assistant/ItemWidgets/SToggleVisibility.h>
#include <Assistant/TreeItems.h>
#include <Assistant/TreeItemVisitors.h>
#include <LevelStreamAssistantStyle.h>
#include <Widgets/Input/SButton.h>
#include <Widgets/Images/SImage.h>
#include <EditorStyleSet.h>
#include <Engine/LevelStreaming.h>
#include <Engine/Level.h>
#include <ScopedTransaction.h>
#include <Utilities/LevelStreamAssistantUtils.h>
#include <Iterators/ItemIterators.h>
#include <EditorLevelUtils.h>
#include <Framework/Application/SlateApplication.h>

#define LOCTEXT_NAMESPACE "LevelStreamAssistant.SToggleVisibility"

namespace LevelStreamAssistant
{
	void SToggleVisibility::Construct(const FArguments& InArgs, TSharedRef<FBaseItem> InItem, const TArray<TSharedPtr<FBaseItem>>* InHierarchy)
	{
		Hierarchy = InHierarchy;
		Item = InItem;

		if (Item.IsValid())
		{
			TSharedPtr<SButton> LevelVisibilityButton;
			TSharedPtr<SImage> LevelVisibilityImage;

			ChildSlot
			[
				// toggle level visibility
				SAssignNew(LevelVisibilityButton, SButton)
				.ButtonStyle(FEditorStyle::Get(), "ToggleButton")
				.OnClicked(this, &SToggleVisibility::OnClicked_ToggleVisibilityButton)
				.ToolTipText(LOCTEXT("ToolTipToggleLevelVisibility", "Toggle Level Visibility"))
				.ContentPadding(0)
				.Content()
				[
					SAssignNew(LevelVisibilityImage, SImage)
					.Image(this, &SToggleVisibility::GetLevelVisibilityBrush)
				]
			];

			TAttribute<FSlateColor> SizeMapForegroundColor = FLevelStreamAssistantStyle::GetButtonForegroundAttribute(LevelVisibilityButton);
			LevelVisibilityButton->SetForegroundColor(SizeMapForegroundColor);
			LevelVisibilityImage->SetColorAndOpacity(SizeMapForegroundColor);
		}
	}

	FReply SToggleVisibility::OnClicked_ToggleVisibilityButton()
	{
		if (Item.IsValid())
		{
			FScopedTransaction Transaction(LOCTEXT("TransactionToggleLevelVisibility", "Toggle Level Visibility"));

			bool bItemShouldBeVisible = !IsAnyAssociatedLevelVisible(*Item); // toggle visibility

			const TArray<TSharedPtr<FBaseItem>>& HierarchyRoot = { Item };

			for (TSharedPtr<FBaseItem> SubItem : FItemHierarchyRange(HierarchyRoot))
			{
				if (SubItem.IsValid())
				{
					SubItem->Accept(FFunctionalItemVisitor()
						.Level([bItemShouldBeVisible](const FLevelItem& LevelItem)
						{
							SetVisibilityToAssociatedLevel(LevelItem, bItemShouldBeVisible);
						})
						.LevelRoot([bItemShouldBeVisible](const FLevelRootItem& Root)
						{
							SetVisibilityToAssociatedLevel(Root, bItemShouldBeVisible);
						})
					);
				}
			}

			// keeps consistent behavior with editor's levels tab.
			if (Hierarchy && FSlateApplication::Get().GetModifierKeys().AreModifersDown(EModifierKey::Alt))
			{
				bool bItemToSkipIsRoot = ItemIsRoot(*Item);

				TFunction<const TArray<TSharedPtr<FBaseItem>>*(const TSharedPtr<FBaseItem>&)> GetOtherChildren
					= [&ItemToSkip = Item, bItemToSkipIsRoot](const TSharedPtr<FBaseItem>& OtherItem)
				{
					bool bShouldGetChildren = OtherItem != ItemToSkip;
					bShouldGetChildren |= OtherItem == ItemToSkip && bItemToSkipIsRoot; // don't skip root.
					return bShouldGetChildren && OtherItem.IsValid() ? &OtherItem->Children : nullptr;
				};

				for (TSharedPtr<FBaseItem> OtherItem : THierarchyRange<TSharedPtr<FBaseItem>>(*Hierarchy, GetOtherChildren))
				{
					if (OtherItem.IsValid() && OtherItem != Item)
					{
						OtherItem->Accept(FFunctionalItemVisitor()
							.Level([bItemShouldBeVisible](const FLevelItem& LevelItem)
							{
								SetVisibilityToAssociatedLevel(LevelItem, !bItemShouldBeVisible);
							})
							.LevelRoot([bItemShouldBeVisible](const FLevelRootItem& Root)
							{
								SetVisibilityToAssociatedLevel(Root, !bItemShouldBeVisible);
							})
						);
					}
				};
			}

			LevelStreamAssistantUtils::FlushLevelStreaming();
		}

		return FReply::Handled();
	}

	const FSlateBrush* SToggleVisibility::GetLevelVisibilityBrush() const
	{
		if (Item.IsValid())
		{
			if (IsAnyAssociatedLevelVisible(*Item))
			{
				return FLevelStreamAssistantStyle::Get().GetBrush("LSA.Level.Visible");
			}
			else
			{
				return FLevelStreamAssistantStyle::Get().GetBrush("LSA.Level.Hidden");
			}
		}

		return FEditorStyle::GetBrush("Level.EmptyIcon16x");
	}
	
	bool SToggleVisibility::IsAnyAssociatedLevelVisible(const FBaseItem& Item)
	{
		bool bVisible = false;

		FFunctionalItemVisitor GetLevelVisibility;
		GetLevelVisibility = FFunctionalItemVisitor()
			.Level([&bVisible](const FLevelItem& LevelItem)
			{
				if (ULevelStreaming* Level = LevelItem.Level.Get())
				{
					bVisible = Level->IsLevelVisible();
				}
			})
			.LevelRoot([&bVisible](const FLevelRootItem& Root)
			{
				if (ULevel* Level = Root.PersistentLevel.Get())
				{
					bVisible = Level->bIsVisible;
				}
			})
			.LevelFolder([&GetLevelVisibility, &bVisible](const FLevelFolderItem& Folder)
			{		
				for (TSharedPtr<FBaseItem> SubItem : Folder.Children)
				{
					if (SubItem.IsValid())
					{
						SubItem->Accept(GetLevelVisibility);
						if (bVisible) break;
					}
				}
			});

		Item.Accept(GetLevelVisibility);
	
		return bVisible;
	}

	void SToggleVisibility::SetVisibilityToAssociatedLevel(const FLevelRootItem& Root, bool bShouldBeVisible)
	{
		if (ULevel* PersistentLevel = Root.PersistentLevel.Get())
		{
			EditorLevelUtils::SetLevelVisibility(PersistentLevel, bShouldBeVisible, false, ELevelVisibilityDirtyMode::DontModify);
		}
	}

	void SToggleVisibility::SetVisibilityToAssociatedLevel(const FLevelItem& LevelItem, bool bShouldBeVisible)
	{
		if (ULevelStreaming* Level = LevelItem.Level.Get())
		{
			Level->SetShouldBeVisibleInEditor(bShouldBeVisible);
		}
	}

	bool SToggleVisibility::ItemIsRoot(const FBaseItem& Item)
	{
		bool bIsRoot = false;
		Item.Accept(FFunctionalItemVisitor()
			.LevelRoot([&bIsRoot](const FLevelRootItem& Root)
			{
				bIsRoot = true;
			})
		);
		return bIsRoot;
	}
}

#undef LOCTEXT_NAMESPACE
