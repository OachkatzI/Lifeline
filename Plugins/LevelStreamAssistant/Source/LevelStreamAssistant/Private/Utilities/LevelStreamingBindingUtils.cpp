// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Utilities/LevelStreamingBindingUtils.h"
#include <Assistant/LevelStreamingEditor.h>
#include <Assistant/TreeItems.h>
#include <Assistant/TreeItemVisitors.h>
#include <Iterators/WeakPointerIterator.h>
#include <Iterators/ItemIterators.h>
#include <SingleValue.h>

#define LOCTEXT_NAMESPACE "LevelStreamingBindingUtils"

namespace LevelStreamAssistant
{
	EBindingModeResult GetItemBindingModeToSelectedActors(TSharedPtr<ILevelStreamingEditor> LevelStreamingEditor, TSharedPtr<FBaseItem> Item, TSharedPtr<FLevelStreamBindingMode>& OutBindingMode)
	{
		TSingleValue<TSharedPtr<FLevelStreamBindingMode>> BindingMode;

		for (AActor* LevelStreamingActor : TWeakArraySafeRange<AActor>(LevelStreamingEditor->GetSelectedLevelStreamingActors()))
		{
			const TArray<TSharedPtr<FBaseItem>>& HierarchyRoot = { Item };
			for (TSharedPtr<FBaseItem> SubItem : FItemHierarchyRange(HierarchyRoot))
			{
				if(SubItem.IsValid())
				{
					SubItem->Accept(FFunctionalItemVisitor()
						.Level([&BindingMode, &LevelStreamingEditor, LevelStreamingActor](const FLevelItem& LevelItem)
						{
							BindingMode.AddNextValue(LevelStreamingEditor->GetBinding(LevelStreamingActor, LevelItem.Level.Get()));
						})
					);

					if (BindingMode.HasDifferentValues()) break;
				}
			}

			if (BindingMode.HasDifferentValues()) break;
		}

		OutBindingMode = BindingMode.GetValue(nullptr);

		return OutBindingMode.IsValid() ? EBindingModeResult::Success
			: BindingMode.HasDifferentValues() ? EBindingModeResult::MultipleBindings
			: EBindingModeResult::NoBinding;
    }

	EBindingModeResult GetBindingModeToSelectedActors(TSharedPtr<ILevelStreamingEditor> LevelStreamingEditor, ULevelStreaming* Level, TSharedPtr<FLevelStreamBindingMode>& OutBindingMode)
	{
		TSingleValue<TSharedPtr<FLevelStreamBindingMode>> BindingMode;

		for (AActor* LevelStreamingActor : TWeakArraySafeRange<AActor>(LevelStreamingEditor->GetSelectedLevelStreamingActors()))
		{
			BindingMode.AddNextValue(LevelStreamingEditor->GetBinding(LevelStreamingActor, Level));

			if (BindingMode.HasDifferentValues()) break;
		}

		OutBindingMode = BindingMode.GetValue(nullptr);

		return OutBindingMode.IsValid() ? EBindingModeResult::Success
			: BindingMode.HasDifferentValues() ? EBindingModeResult::MultipleBindings
			: EBindingModeResult::NoBinding;
	}

	void SetItemBindingModeToSelectedActors(TSharedPtr<ILevelStreamingEditor> LevelStreamingEditor, TSharedPtr<FBaseItem> Item, TSharedPtr<FLevelStreamBindingMode> NewBindingMode)
	{
		for (AActor* LevelStreamingActor : TWeakArraySafeRange<AActor>(LevelStreamingEditor->GetSelectedLevelStreamingActors()))
		{
			const TArray<TSharedPtr<FBaseItem>>& HierarchyRoot = { Item };
			for (TSharedPtr<FBaseItem> SubItem : FItemHierarchyRange(HierarchyRoot))
			{
				if (SubItem.IsValid())
				{
					SubItem->Accept(FFunctionalItemVisitor()
						.Level([LevelStreamingActor, &LevelStreamingEditor, &NewBindingMode](const FLevelItem& LevelItem) {
							LevelStreamingEditor->SetBinding(LevelStreamingActor, LevelItem.Level.Get(), NewBindingMode);
						})
					);
				}
			}
		}
	}

	void SetBindingModeToSelectedActors(TSharedPtr<ILevelStreamingEditor> LevelStreamingEditor, ULevelStreaming* Level, TSharedPtr<FLevelStreamBindingMode> NewBindingMode)
	{
		for (AActor* LevelStreamingActor : TWeakArraySafeRange<AActor>(LevelStreamingEditor->GetSelectedLevelStreamingActors()))
		{
			LevelStreamingEditor->SetBinding(LevelStreamingActor, Level, NewBindingMode);
		}
	}

	TSharedPtr<FLevelStreamBindingMode> GetFirstBindingMode(TSharedPtr<ILevelStreamingEditor> LevelStreamingEditor)
	{
		for (TSharedPtr<FLevelStreamBindingMode> BindingMode : LevelStreamingEditor->GetBindingModes())
		{
			if (BindingMode.IsValid()) return BindingMode;
		}
		return nullptr;
	}

	bool IsLevelBoundToAnySelectedActor(TSharedPtr<ILevelStreamingEditor> LevelStreamingEditor, ULevelStreaming* InLevel)
	{
		if (InLevel)
		{
			for (AActor* SelectedActor : TWeakArraySafeRange<AActor>(LevelStreamingEditor->GetSelectedLevelStreamingActors()))
			{
				if (LevelStreamingEditor->GetBinding(SelectedActor, InLevel).IsValid())
				{
					return true;
				}
			}
		}
		return false;
	}
}

#undef LOCTEXT_NAMESPACE
