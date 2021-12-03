// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include <Assistant/ItemWidgets/SItemIcon.h>
#include <Assistant/TreeItems.h>
#include <Assistant/TreeItemVisitors.h>
#include <Assistant/Folders/FolderManager.h>
#include <Utilities/LevelStreamAssistantItemUtils.h>
#include <Widgets/Images/SImage.h>
#include <ActorIconFinder.h>
#include <Engine/LevelStreaming.h>
#include <EditorStyleSet.h>

#define LOCTEXT_NAMESPACE "LevelStreamAssistant.SItemIcon"

namespace LevelStreamAssistant
{
	void SItemIcon::Construct(const FArguments& InArgs, const TSharedRef<ITableRow>& InRow, const TSharedRef<FBaseItem>& InItem)
	{
		Row = InRow;
		ActorIconFinder = InArgs._ActorIconFinder;

		InItem->Accept(FFunctionalItemVisitor()
			.Actor([this](const FActorItem& Item)
			{
				ChildSlot
				[
					SNew(SImage).Image(this, &SItemIcon::GetActorIcon, Item.Actor)
				];
			})
			.Level([this](const FLevelItem& Item)
			{
				ChildSlot
				[
					SNew(SImage).Image(this, &SItemIcon::GetLevelIcon, Item.Level)
				];
			})
			.ActorFolder([this](const FActorFolderItem& Item)
			{
				TSharedPtr<const FBaseItem> Folder = Item.AsShared();

				ChildSlot
				[
					SNew(SImage).Image(this, &SItemIcon::GetFolderIcon)
					.ColorAndOpacity(this, &SItemIcon::GetFolderColor, Folder)
				];
			})
			.LevelFolder([this](const FLevelFolderItem& Item)
			{
				TSharedPtr<const FBaseItem> Folder = Item.AsShared();

				ChildSlot
				[
					SNew(SImage).Image(this, &SItemIcon::GetFolderIcon)
					.ColorAndOpacity(this, &SItemIcon::GetFolderColor, Folder)
				];
			})
		);
	}

	const FSlateBrush* SItemIcon::GetActorIcon(TWeakObjectPtr<AActor> Actor) const
	{
		if (ActorIconFinder.IsValid())
		{
			return ActorIconFinder->FindActorIcon(Actor);
		}
		return nullptr;
	}

	const FSlateBrush* SItemIcon::GetLevelIcon(TWeakObjectPtr<ULevelStreaming> Level) const
	{
		if (Level.IsValid())
		{
			return Level->ShouldBeAlwaysLoaded()
				? FEditorStyle::GetBrush("WorldBrowser.LevelStreamingAlwaysLoaded")
				: FEditorStyle::GetBrush("WorldBrowser.LevelStreamingBlueprint");
		}
		return nullptr;
	}

	const FSlateBrush* SItemIcon::GetFolderIcon() const
	{
		return Row.IsValid() && Row.Pin()->DoesItemHaveChildren() && Row.Pin()->IsItemExpanded()
			? FEditorStyle::Get().GetBrush(TEXT("SceneOutliner.FolderOpen"))
			: FEditorStyle::Get().GetBrush(TEXT("SceneOutliner.FolderClosed"));
	}

	FSlateColor SItemIcon::GetFolderColor(TSharedPtr<const FBaseItem> FolderItem) const
	{
		if (FolderItem.IsValid())
		{
			if (FFolderProperties* FolderProperties = LevelStreamAssistantUtils::GetItemFolderProperties(*FolderItem))
			{
				return FolderProperties->Color;
			}
		}
		return FLinearColor::White;
	}
}

#undef LOCTEXT_NAMESPACE
