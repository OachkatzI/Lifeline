// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class FReply;
class AActor;

class SDragDropActor : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDragDropActor) :
		_ActorType(AActor::StaticClass())
	{}
		SLATE_ATTRIBUTE(TSubclassOf<AActor>, ActorType)
		SLATE_ATTRIBUTE(const FSlateBrush*, Image)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	FText GetDragAndDropActorToolTip() const;

	TAttribute<TSubclassOf<AActor>> ActorType;
};
