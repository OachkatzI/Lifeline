// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include <Assistant/DragAndDrop/TreeItemDragDropOp.h>
#include <Assistant/TreeItems.h>

#define LOCTEXT_NAMESPACE "LevelStreamAssistant.FTreeItemDragDropOp"

namespace LevelStreamAssistant
{
	void FTreeItemDragDropOp::Init()
	{
		if (ItemsToDrop.Num() == 1)
		{
			if (ItemsToDrop[0].IsValid())
			{
				CurrentHoverText = FText::FromString(FString::Printf(TEXT("Drag and drop %s"), *ItemsToDrop[0]->GetLabel()));
			}
		}
		else if (ItemsToDrop.Num() > 1)
		{
			CurrentHoverText = FText::FromString(FString::Printf(TEXT("Drag and drop %i items"), ItemsToDrop.Num()));
		}

		SetupDefaults();
	}

	TSharedRef<FTreeItemDragDropOp> FTreeItemDragDropOp::New(const TArray<TSharedPtr<FBaseItem>>& InItemsToDrop)
	{
		TSharedRef<FTreeItemDragDropOp> Operation = MakeShareable(new FTreeItemDragDropOp);
		Operation->ItemsToDrop = InItemsToDrop;
		Operation->Init();
		Operation->Construct();
		return Operation;
	}
}

#undef LOCTEXT_NAMESPACE
