// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Widgets/SCompoundWidget.h>
#include <Widgets/DeclarativeSyntaxSupport.h>

namespace LevelStreamAssistant
{
	class ILevelStreamingEditor;
	struct FBaseItem;

	// change level binding (only for folders and levels).
	class SLevelBinding : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SLevelBinding) {}
		SLATE_EVENT(FSimpleDelegate, OnHovered)
		SLATE_EVENT(FSimpleDelegate, OnUnhovered)
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, TSharedRef<FBaseItem> InItem, TSharedRef<ILevelStreamingEditor> InLevelStreamingEditor);

	private:
		TSharedPtr<FBaseItem> Item;
		TSharedPtr<ILevelStreamingEditor> LevelStreamingEditor;

		bool IsAnyActorSelected() const;

		FReply OnClicked_ModifyLevelBinding();

		void ToggleBindingMode();
		void SwitchToNextBindingMode();

		EVisibility OnGetVisibility() const;
		static bool LevelCanBeStreamed(const FBaseItem& Item);
	};
}
