// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "SelectionTool/ActorSelectorBoxCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"

#include "ScopedTransaction.h"
#include "Editor.h"

#include "SelectionTool/ActorSelectorBox.h"
#include "EngineUtils.h"

#include <Utilities/LevelStreamAssistantUtils.h>

#include <Components/MeshComponent.h>

#include "TimerManager.h"

#define LOCTEXT_NAMESPACE "FActorSelectorBoxCustomization"

TSharedRef<IDetailCustomization> FActorSelectorBoxCustomization::MakeInstance()
{
	return MakeShareable(new FActorSelectorBoxCustomization);
}

void FActorSelectorBoxCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	DetailLayout.EditCategory("Tools", FText::GetEmpty(), ECategoryPriority::Important)
		.AddCustomRow(LOCTEXT("Select", "Select Actors Inside"))
		[
			SNew(SButton)
			.HAlign(HAlign_Center)
			.OnClicked(this, &FActorSelectorBoxCustomization::OnClickedSelect)
			.Content()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("LabelSelect", "Select Actors Inside"))
			]
		];
}

void FActorSelectorBoxCustomization::CustomizeDetails(const TSharedPtr<IDetailLayoutBuilder>& DetailBuilder)
{
	DetailBuilderWeakPtr = DetailBuilder;
	CustomizeDetails(*DetailBuilder);
}

FReply FActorSelectorBoxCustomization::OnClickedSelect()
{
	TSharedPtr<IDetailLayoutBuilder> DetailBuilder = DetailBuilderWeakPtr.Pin();
	if (DetailBuilder.IsValid() && GEditor)
	{
		if (UWorld* World = GEditor->GetEditorWorldContext().World())
		{
			TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
			DetailBuilder->GetObjectsBeingCustomized(ObjectsBeingCustomized);

			FScopedTransaction Transaction(LOCTEXT("TransactionSelect", "Volume Select"));

			// selection is deffered to next tick to prevent editor crash.
			const bool bNoteSelectionChange = false;

			GEditor->SelectNone(bNoteSelectionChange, true);

			TActorRange<AActor> IterateValidActorsIncludingHiddenActors(World, AActor::StaticClass(), EActorIteratorFlags::SkipPendingKill);
			for (AActor* Actor : IterateValidActorsIncludingHiddenActors)
			{
				if (IsValid(Actor) && !Actor->IsA<AActorSelectorBox>())
				{
					bool bActorContainsMesh = Actor->FindComponentByClass<UMeshComponent>() != nullptr;

					FBox ActorBox = LevelStreamAssistantUtils::CalculateActorBounds(Actor, /*bIgnoreBoundsExtension*/true);

					for (TWeakObjectPtr<UObject> CustomizedObject : ObjectsBeingCustomized)
					{
						if (AActorSelectorBox* ActorSelector = Cast<AActorSelectorBox>(CustomizedObject.Get()))
						{
							FBox VolumeBox = ActorSelector->GetComponentsBoundingBox();
							FBox OverlapBox = VolumeBox.Overlap(ActorBox);

							float OverlapPercentage = OverlapBox.GetVolume() / ActorBox.GetVolume();

							bool bActorOverlapsVolume = ActorBox.Intersect(VolumeBox);
							bool bIsUnderThreshold = OverlapPercentage >= ActorSelector->SelectOverlapThreshold;
							bool bActorIsLargerThanVolume = ActorBox.GetVolume() > VolumeBox.GetVolume();
							bool bExcludedByLargeSize = ActorSelector->bExcludeActorsLargerThanThisVolume && bActorIsLargerThanVolume;
							bool bExcludedByNotHavingMesh = ActorSelector->bOnlySelectActorsWithMesh && !bActorContainsMesh;

							bool bShouldSelectActor = bActorOverlapsVolume && bIsUnderThreshold && !bExcludedByLargeSize && !bExcludedByNotHavingMesh;

							bool bSelectEvenIfHidden = ActorSelector->bSelectHiddenActors;

							if (bShouldSelectActor)
							{
								GEditor->SelectActor(Actor, true, bNoteSelectionChange, bSelectEvenIfHidden);
								break;
							}
						}
					}
				}
			}

			// note editor selection change on next tick.
			World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateSP(this, &FActorSelectorBoxCustomization::NotifySelectionChange));
		}
	}

	return FReply::Handled();
}

void FActorSelectorBoxCustomization::NotifySelectionChange()
{
	if (GEditor) GEditor->NoteSelectionChange();
}

#undef LOCTEXT_NAMESPACE