// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SWarning : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_RetVal(bool, FShouldBeFixedDelegate);

	SLATE_BEGIN_ARGS(SWarning) { }
		SLATE_EVENT(FSimpleDelegate, OnFix)
		SLATE_EVENT(FShouldBeFixedDelegate, ShouldBeFixed) // return true if there is a problem that should be fixed.
		SLATE_ATTRIBUTE(FText, WarningText)
		SLATE_ATTRIBUTE(FText, DismissText)
		SLATE_ATTRIBUTE(FText, FixText)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FReply OnFix();
	FReply OnDismiss();

	FSimpleDelegate FixEvent;
	FShouldBeFixedDelegate ShouldBeFixed;

	EVisibility GetWarningVisibility() const;
	
	EActiveTimerReturnType UpdateWarningVisibility(double InCurrentTime, float InDeltaTime);

	EVisibility WarningVisibility = EVisibility::Collapsed;

	bool bDismissed = false;
};
