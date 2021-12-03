// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include <Assistant/ItemWidgets/SLevelBinding.h>
#include <Assistant/LevelStreamingEditor.h>
#include <Assistant/TreeItems.h>
#include <Assistant/TreeItemVisitors.h>
#include <Widgets/Input/SButton.h>
#include <Widgets/Layout/SBox.h>
#include <Widgets/Images/SImage.h>
#include <EditorStyleSet.h>
#include <LevelStreamBindingMode.h>
#include <LevelStreamAssistantStyle.h>
#include <ScopedTransaction.h>
#include <Utilities/LevelStreamingBindingUtils.h>

#define LOCTEXT_NAMESPACE "LevelStreamAssistant.SLevelBinding"

namespace LevelStreamAssistant
{
	void SLevelBinding::Construct(const FArguments& InArgs, TSharedRef<FBaseItem> InItem, TSharedRef<ILevelStreamingEditor> InLevelStreamingEditor)
	{
		Item = InItem;
		LevelStreamingEditor = InLevelStreamingEditor;

		FLinearColor BindingModeColor;
		const FSlateBrush* BindingModeBrush = nullptr;

		TSharedPtr<FLevelStreamBindingMode> BindingMode;
		switch (GetItemBindingModeToSelectedActors(LevelStreamingEditor, InItem, BindingMode))
		{
		case EBindingModeResult::Success:
			BindingModeBrush = FEditorStyle::GetBrush("WhiteBrush");
			BindingModeColor = BindingMode->GetColor().CopyWithNewOpacity(1.0f); // alpha is not used here.
			break;

		case EBindingModeResult::MultipleBindings:
			BindingModeBrush = FLevelStreamAssistantStyle::Get().GetBrush("LSA.Binding.Undetermined");
			BindingModeColor = FLinearColor::White;
			break;

		case EBindingModeResult::NoBinding:
		default:
			BindingModeBrush = FEditorStyle::GetBrush("WhiteBrush");
			BindingModeColor = FLinearColor::Transparent;
			break;
		}

		ChildSlot
		[
			// change level binding mode.
			SNew(SButton)
			.Visibility(this, &SLevelBinding::OnGetVisibility)
			.IsEnabled(this, &SLevelBinding::IsAnyActorSelected)
			.ButtonStyle(FEditorStyle::Get(), "ToggleButton")
			.OnClicked(this, &SLevelBinding::OnClicked_ModifyLevelBinding)
			.OnHovered(InArgs._OnHovered)
			.OnUnhovered(InArgs._OnUnhovered)
			.ContentPadding(0)
			.Content()
			[
				SNew(SBox)
				.WidthOverride(16)
				.HeightOverride(16)
				[
					SNew(SBorder)
					.Padding(0.f)
					.BorderImage(BindingModeBrush)
					.BorderBackgroundColor(BindingModeColor)
					[
						SNew(SImage)
						.Image(FLevelStreamAssistantStyle::Get().GetBrush("LSA.Binding.Unset"))
					]
				]
			]
		];
	}

	bool SLevelBinding::IsAnyActorSelected() const
	{
		return LevelStreamingEditor->GetSelectedLevelStreamingActors().Num() > 0;
	}

	FReply SLevelBinding::OnClicked_ModifyLevelBinding()
	{
		FScopedTransaction Transaction(LOCTEXT("TransactionModifyLevelBinding", "Modify Level Binding"));
		{
			if (FSlateApplication::Get().GetModifierKeys().AreModifersDown(EModifierKey::Alt))
			{
				ToggleBindingMode();
			}
			else
			{
				SwitchToNextBindingMode();
			}

			LevelStreamingEditor->NotifyLevelBindingChanged();
		}

		return FReply::Handled();
	}
	
	void SLevelBinding::ToggleBindingMode()
	{
		TSharedPtr<FLevelStreamBindingMode> BindingMode;
		switch (GetItemBindingModeToSelectedActors(LevelStreamingEditor, Item, BindingMode))
		{
		case EBindingModeResult::Success:
		case EBindingModeResult::MultipleBindings:
			// clear all bindings.
			SetItemBindingModeToSelectedActors(LevelStreamingEditor, Item, /*NewBindingMode*/nullptr);
			break;

		case EBindingModeResult::NoBinding:
		default:
			/// switch to first binding mode.
			TSharedPtr<FLevelStreamBindingMode> FirstBindingMode = LevelStreamAssistant::GetFirstBindingMode(LevelStreamingEditor);
			SetItemBindingModeToSelectedActors(LevelStreamingEditor, Item, FirstBindingMode);
			break;
		}
	}

	void SLevelBinding::SwitchToNextBindingMode()
	{
		TSharedPtr<FLevelStreamBindingMode> BindingMode;
		switch (GetItemBindingModeToSelectedActors(LevelStreamingEditor, Item, BindingMode))
		{
		case EBindingModeResult::MultipleBindings:
			// clear all bindings.
			SetItemBindingModeToSelectedActors(LevelStreamingEditor, Item, /*NewBindingMode*/nullptr);
			break;

		case EBindingModeResult::Success:
		case EBindingModeResult::NoBinding:
		default:
			/// switch to next binding mode.
			const TArray<TSharedPtr<FLevelStreamBindingMode>>& BindingModes = LevelStreamingEditor->GetBindingModes();
			int32 IndexOfNextBindingMode = 1 + BindingModes.Find(BindingMode);

			TSharedPtr<FLevelStreamBindingMode> NextBindingMode = BindingModes.IsValidIndex(IndexOfNextBindingMode)
				? BindingModes[IndexOfNextBindingMode]
				: nullptr;

			SetItemBindingModeToSelectedActors(LevelStreamingEditor, Item, NextBindingMode);
			break;
		}
	}

	EVisibility SLevelBinding::OnGetVisibility() const
	{
		return Item.IsValid() && LevelCanBeStreamed(*Item)
			? EVisibility::Visible
			: EVisibility::Collapsed;
	}

	bool SLevelBinding::LevelCanBeStreamed(const FBaseItem& Item)
	{
		bool bCanBeStreamed = false;
		FFunctionalItemVisitor CheckCanBeStreamed;
		CheckCanBeStreamed = FFunctionalItemVisitor()
		.Level([&bCanBeStreamed](const FLevelItem& Item)
		{
			bCanBeStreamed = Item.Level.IsValid();
		})
		.LevelFolder([&CheckCanBeStreamed, &bCanBeStreamed](const FLevelFolderItem& Item)
		{
			for (TSharedPtr<FBaseItem> SubItem : Item.Children)
			{
				if (SubItem.IsValid())
				{
					SubItem->Accept(CheckCanBeStreamed);
					if (bCanBeStreamed) break;
				}
			}
		});
		Item.Accept(CheckCanBeStreamed);
		return bCanBeStreamed;
	}
}

#undef LOCTEXT_NAMESPACE
