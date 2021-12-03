// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

#include "LevelStreamAssistantStyle.h"
#include "Widgets/Input/SSearchBox.h"
#include "Misc/TextFilter.h"

// contains a search box widget and provides a simple way to filter items using TTextFilter.
template<typename ItemType>
class SSimpleSearchBox : public SCompoundWidget
{
public:
	using FItemTextFilter = typename TTextFilter<ItemType>;
	using FItemToStringArray = typename TTextFilter<ItemType>::FItemToStringArray;

public:
	SLATE_BEGIN_ARGS(SSimpleSearchBox) {}
		SLATE_EVENT(FItemToStringArray, ItemToStringArray)
		SLATE_EVENT(FSimpleDelegate, OnSearchChanged)
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs)
	{
		Delegate_ItemToStringArray = InArgs._ItemToStringArray;
		Delegate_OnSearchChanged = InArgs._OnSearchChanged;

		// filters used to search for items.
		ItemFilter = MakeShareable(new FItemTextFilter(FItemToStringArray::CreateSP(this, &SSimpleSearchBox::ItemToStringArray)));

		// notify when search term is changed.
		ItemFilter->OnChanged().AddSP(this, &SSimpleSearchBox::OnSearchChanged);

		ChildSlot
		[
			SAssignNew(SearchBox, SSearchBox)
			.FlowDirectionPreference(EFlowDirectionPreference::RightToLeft)
			.Style(&FLevelStreamAssistantStyle::Get().GetWidgetStyle<FSearchBoxStyle>("LSA.ListView.SearchBox"))
			.OnTextChanged(this, &SSimpleSearchBox::SearchForItems)
		];
	}

	bool PassesFilter(const ItemType& Item) const
	{
		return ItemFilter.IsValid() && ItemFilter->PassesFilter(Item);
	}

private:
	void SearchForItems(const FText& SearchText)
	{
		if (ItemFilter.IsValid())
		{
			ItemFilter->SetRawFilterText(SearchText);
		}
	}

	void OnSearchChanged()
	{
		if (SearchBox.IsValid() && ItemFilter.IsValid())
		{
			SearchBox->SetError(ItemFilter->GetFilterErrorText());
		}

		Delegate_OnSearchChanged.ExecuteIfBound();
	}

	void ItemToStringArray(ItemType InItem, TArray<FString>& Array) const
	{
		Delegate_ItemToStringArray.ExecuteIfBound(InItem, Array);
	}

	TSharedPtr<SSearchBox> SearchBox;
	TSharedPtr<FItemTextFilter> ItemFilter;

	FItemToStringArray Delegate_ItemToStringArray;
	FSimpleDelegate Delegate_OnSearchChanged;
};
