// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Widgets/SCompoundWidget.h>
#include "Widgets/DeclarativeSyntaxSupport.h"

// simple widget that provides hover and unhover events.
class SHoverBox : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SHoverBox)
	{}
		SLATE_DEFAULT_SLOT(FArguments, Content)

		SLATE_EVENT(FSimpleDelegate, OnHovered)
		SLATE_EVENT(FSimpleDelegate, OnUnhovered)
		
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	virtual void OnMouseLeave(const FPointerEvent& MouseEvent);

	~SHoverBox();

private:
	FSimpleDelegate OnHovered;
	FSimpleDelegate OnUnhovered;
};
