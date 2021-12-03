// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include <Assistant/ItemWidgets/SItemLabel.h>
#include <Assistant/TreeItems.h>
#include <Assistant/TreeItemVisitors.h>
#include <Widgets/Text/SInlineEditableTextBlock.h>
#include <Widgets/Text/STextBlock.h>
#include <EditorStyleSet.h>
#include <Engine/LevelStreaming.h>
#include <Engine/Level.h>

#define LOCTEXT_NAMESPACE "LevelStreamAssistant.SItemLabel"

namespace LevelStreamAssistant
{
	void SItemLabel::Construct(const FArguments& InArgs, TSharedRef<ITableRow> InRow, TSharedRef<FBaseItem> InItem)
	{
		Row = InRow;
		Item = InItem;
		OnLabelEdited = InArgs._OnLabelEdited;

		bool bIsEditable = false;
		Item->Accept(FFunctionalItemVisitor()
			.Actor([&bIsEditable](const FActorItem& ActorItem) { bIsEditable = true; })
			.ActorFolder([&bIsEditable](const FActorFolderItem& Folder) { bIsEditable = true; })
			.LevelFolder([&bIsEditable](const FLevelFolderItem& Level) { bIsEditable = true; })
		);

		if (bIsEditable)
		{
			TSharedPtr<SInlineEditableTextBlock> EditableText;

			ChildSlot
			[
				SAssignNew(EditableText, SInlineEditableTextBlock)
				.IsSelected(this, &SItemLabel::IsSelected)
				.OnTextCommitted(this, &SItemLabel::OnTextCommitted)
				.OnVerifyTextChanged(this, &SItemLabel::OnVerifyText)
				.Font(this, &SItemLabel::GetLabelFont)
				.Text(FText::FromString(Item->GetLabel()))
				.ToolTipText(Item->GetToolTip())
			];

			Item->OnRequestRename.BindSP(EditableText.ToSharedRef(), &SInlineEditableTextBlock::EnterEditingMode);
		}
		else
		{
			ChildSlot
			[
				SNew(STextBlock)
				.Font(this, &SItemLabel::GetLabelFont)
				.Text(FText::FromString(Item->GetLabel()))
				.ToolTipText(Item->GetToolTip())
			];
		}
	}

	void SItemLabel::OnTextCommitted(const FText& Text, ETextCommit::Type CommitType) const
	{
		if (CommitType != ETextCommit::OnCleared)
		{
			OnLabelEdited.ExecuteIfBound(Text);
		}
	}

	bool SItemLabel::OnVerifyText(const FText& InText, FText& OutError) const
	{
		bool bCanContainSlash = false;

		if (Item.IsValid())
		{
			Item->Accept(FFunctionalItemVisitor()
				.Actor([&bCanContainSlash](const FActorItem& Actor) { bCanContainSlash = true; })
			);
		}

		FString TextStr = InText.ToString();
		if (!bCanContainSlash && (TextStr.Contains(TEXT("/")) || TextStr.Contains(TEXT("\\"))))
		{
			OutError = LOCTEXT("ErrorNameContainsSlashOrBackSlash", "Name can not contain '/' or '\\'.");
			return false;
		}
		if (TextStr.Equals(TEXT("None"), ESearchCase::IgnoreCase))
		{
			OutError = LOCTEXT("ErrorNameIsNone", "Name can not be \"None\".");
			return false;
		}
		if (TextStr.TrimStartAndEnd().IsEmpty())
		{
			OutError = LOCTEXT("ErrorNameIsBlank", "Name can not be blank.");
			return false;
		}

		return true;
	}

	bool SItemLabel::IsSelected() const
	{
		return Row.IsValid() && Row.Pin()->IsItemSelected();
	}

	FSlateFontInfo SItemLabel::GetLabelFont() const
	{
		FSlateFontInfo Font = FEditorStyle::GetFontStyle("WorldBrowser.LabelFont");
		if (Item.IsValid())
		{
			Item->Accept(FFunctionalItemVisitor()
				.Level([&Font](const FLevelItem& LevelItem)
				{
					if(LevelItem.Level.IsValid() && 
						LevelItem.Level->IsLevelLoaded() && 
						LevelItem.Level->GetLoadedLevel()->IsCurrentLevel())
					{
						Font = FEditorStyle::GetFontStyle("WorldBrowser.LabelFontBold");
					}
				})
				.LevelRoot([&Font](const FLevelRootItem& LevelItem)
				{
					if (LevelItem.PersistentLevel.IsValid() && 
						LevelItem.PersistentLevel->IsCurrentLevel())
					{
						Font = FEditorStyle::GetFontStyle("WorldBrowser.LabelFontBold");
					}
				})
			);
		}
		return Font;
	}
}

#undef LOCTEXT_NAMESPACE
