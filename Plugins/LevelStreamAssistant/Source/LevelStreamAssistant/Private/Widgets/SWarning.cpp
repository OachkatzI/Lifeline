// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Widgets/SWarning.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "LevelStreamAssistantStyle.h"
#include "EditorStyleSet.h"

void SWarning::Construct(const FArguments& InArgs)
{
	FixEvent = InArgs._OnFix;
	ShouldBeFixed = InArgs._ShouldBeFixed;

	TSharedPtr<SButton> FixButton;
	TSharedPtr<SButton> DismissButton;
	
	ChildSlot
	[
		SNew(SBorder)
		.Visibility(this, &SWarning::GetWarningVisibility)
		.BorderImage(FEditorStyle::GetBrush("SettingsEditor.CheckoutWarningBorder"))
		.BorderBackgroundColor(FColor(200, 129, 26))
		.Padding(FMargin(2.f))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(InArgs._WarningText)
				.AutoWrapText(true)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(DismissButton, SButton)
				.Text(InArgs._DismissText)
				.OnClicked(this, &SWarning::OnDismiss)
				.ButtonStyle(FEditorStyle::Get(), "ToggleButton")
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(FixButton, SButton)
				.Text(InArgs._FixText)
				.OnClicked(this, &SWarning::OnFix)
				.ButtonStyle(FEditorStyle::Get(), "ToggleButton")
			]
		]
	];

	FixButton->SetForegroundColor(FLevelStreamAssistantStyle::GetButtonForegroundAttribute(FixButton));
	DismissButton->SetForegroundColor(FLevelStreamAssistantStyle::GetButtonForegroundAttribute(DismissButton));

	RegisterActiveTimer(0.5f, FWidgetActiveTimerDelegate::CreateSP(this, &SWarning::UpdateWarningVisibility));
}

FReply SWarning::OnFix()
{
	FixEvent.ExecuteIfBound();
	return FReply::Handled();
}

FReply SWarning::OnDismiss()
{
	bDismissed = true;
	return FReply::Handled();
}

EVisibility SWarning::GetWarningVisibility() const
{
	return WarningVisibility;
}

EActiveTimerReturnType SWarning::UpdateWarningVisibility(double InCurrentTime, float InDeltaTime)
{
	// collapse this widget if problem is fixed or warning is dismissed.
	WarningVisibility = bDismissed || !ShouldBeFixed.IsBound() || !ShouldBeFixed.Execute()
		? EVisibility::Collapsed
		: EVisibility::SelfHitTestInvisible;

	return EActiveTimerReturnType::Continue;
}
