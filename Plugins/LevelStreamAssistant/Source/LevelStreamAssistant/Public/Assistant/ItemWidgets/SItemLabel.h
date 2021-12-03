// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Widgets/SCompoundWidget.h>
#include <Widgets/DeclarativeSyntaxSupport.h>

class ITableRow;
namespace ETextCommit { enum Type; }
struct FSlateFontInfo;

namespace LevelStreamAssistant
{
	struct FBaseItem;

	// a label for FBaseItem.
	class SItemLabel : public SCompoundWidget
	{
		DECLARE_DELEGATE_OneParam(FOnLabelEdited, const FText&);

	public:
		SLATE_BEGIN_ARGS(SItemLabel) {}
			SLATE_EVENT(FOnLabelEdited, OnLabelEdited)
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, TSharedRef<ITableRow> InRow, TSharedRef<FBaseItem> InItem);

	private:
		void OnTextCommitted(const FText& Text, ETextCommit::Type CommitType) const;
		bool OnVerifyText(const FText& InText, FText& OutError) const;
		bool IsSelected() const;
		FSlateFontInfo GetLabelFont() const;

		TWeakPtr<ITableRow> Row;
		TSharedPtr<FBaseItem> Item;

		FOnLabelEdited OnLabelEdited;
	};
}
