// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Widgets/SHoverBox.h"

#define LOCTEXT_NAMESPACE "LevelStreamAssistant.SHoverBox"

void SHoverBox::Construct(const FArguments& InArgs)
{
	OnHovered = InArgs._OnHovered;
	OnUnhovered = InArgs._OnUnhovered;

	ChildSlot
	[
		InArgs._Content.Widget
	];
}

void SHoverBox::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	bIsHovered = true;
	OnHovered.ExecuteIfBound();
}

void SHoverBox::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	bIsHovered = false;
	OnUnhovered.ExecuteIfBound();
}

SHoverBox::~SHoverBox()
{
	if (bIsHovered)
	{
		OnUnhovered.ExecuteIfBound();
	}
}

#undef LOCTEXT_NAMESPACE
