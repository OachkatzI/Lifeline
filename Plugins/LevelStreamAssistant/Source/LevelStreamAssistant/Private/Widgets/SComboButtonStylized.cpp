// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Widgets/SComboButtonStylized.h"
#include "LevelStreamAssistantStyle.h"

#define LOCTEXT_NAMESPACE "LevelStreamAssistant.SComboButtonStylized"

void SComboButtonStylized::Construct(const FArguments& InArgs)
{
	SComboButton::Construct(SComboButton::FArguments()
		.ComboButtonStyle(InArgs._ComboButtonStyle)
		.ButtonStyle(InArgs._ButtonStyle)
		.ButtonContent()
		[
			InArgs._ButtonContent.Widget
		]
		.MenuContent()
		[
			InArgs._MenuContent.Widget
		]
		.OnGetMenuContent(InArgs._OnGetMenuContent)
		.OnMenuOpenChanged(InArgs._OnMenuOpenChanged)
		.OnComboBoxOpened(InArgs._OnComboBoxOpened)
		.IsFocusable(InArgs._IsFocusable)
		.HasDownArrow(InArgs._HasDownArrow)
		.ForegroundColor(FLevelStreamAssistantStyle::GetComboButtonForegroundAttribute(SharedThis(this)))
		.ButtonColorAndOpacity(InArgs._ButtonColorAndOpacity)
		.ContentPadding(InArgs._ContentPadding)
		.MenuPlacement(InArgs._MenuPlacement)
		.HAlign(InArgs._HAlign)
		.VAlign(InArgs._VAlign)
		.Method(InArgs._Method)
		.CollapseMenuOnParentFocus(InArgs._CollapseMenuOnParentFocus)
	);
}

#undef LOCTEXT_NAMESPACE
