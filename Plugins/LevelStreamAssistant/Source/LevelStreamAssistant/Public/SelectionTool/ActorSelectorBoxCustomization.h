// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class IDetailLayoutBuilder;
class FReply;

class FActorSelectorBoxCustomization : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** Called when details should be customized */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

	virtual void CustomizeDetails(const TSharedPtr<IDetailLayoutBuilder>& DetailBuilder) override;

	FReply OnClickedSelect();
	void NotifySelectionChange();

	TWeakPtr<IDetailLayoutBuilder> DetailBuilderWeakPtr;
};
