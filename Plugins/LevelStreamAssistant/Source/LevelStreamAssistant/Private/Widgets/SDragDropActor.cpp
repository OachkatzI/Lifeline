// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Widgets/SDragDropActor.h"
#include "Widgets/Images/SImage.h"
#include "DragAndDrop/AssetDragDropOp.h"

#define LOCTEXT_NAMESPACE "LevelStreamAssistant.SDragDropActor"

void SDragDropActor::Construct(const FArguments& InArgs)
{
	ActorType = InArgs._ActorType;

	ChildSlot
	[
		SNew(SImage)
		.Image(InArgs._Image)
		.Cursor(EMouseCursor::GrabHand)
		.ToolTipText(this, &SDragDropActor::GetDragAndDropActorToolTip)
	];
}

FReply SDragDropActor::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return FReply::Handled().DetectDrag(SharedThis(this), MouseEvent.GetEffectingButton());
	}
	return FReply::Unhandled();
}

FReply SDragDropActor::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Unhandled();
}

FReply SDragDropActor::OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	const TSubclassOf<AActor>& ActorTypeToUse = ActorType.Get();
	if (*ActorTypeToUse)
	{
		FAssetData AssetData = FAssetData(ActorTypeToUse);
		return FReply::Handled().BeginDragDrop(FAssetDragDropOp::New(AssetData));
	}
	return FReply::Unhandled();
}

FText SDragDropActor::GetDragAndDropActorToolTip() const
{
	UClass* ActorClass = *ActorType.Get();
	return ActorClass
		? FText::Format(LOCTEXT("ActorDragAndDropToolTip", "Drag and drop {0}"), FText::FromString(ActorClass->GetName()))
		: FText::GetEmpty();
}

#undef LOCTEXT_NAMESPACE
