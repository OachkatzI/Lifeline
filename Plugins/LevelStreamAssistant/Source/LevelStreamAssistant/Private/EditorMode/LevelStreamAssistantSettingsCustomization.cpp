// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "EditorMode/LevelStreamAssistantSettingsCustomization.h"
#include "EditorMode/LevelStreamAssistantSettings.h"
#include "Utilities/LevelStreamAssistantUtils.h"
#include "LevelStreamAssistantStyle.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/SSimpleSearchBox.h"
#include "Engine/LevelStreaming.h"
#include "Engine/LevelStreamingVolume.h"
#include "Engine/Selection.h"
#include "ScopedTransaction.h"
#include "Builders/CubeBuilder.h"
#include "Editor.h"
#include "Iterators/WeakPointerIterator.h"

#define LOCTEXT_NAMESPACE "LevelStreamAssistantSettingsCustomization"

TSharedRef<IDetailCustomization> FLevelStreamAssistantSettingsCustomization::MakeInstance()
{
	return MakeShareable(new FLevelStreamAssistantSettingsCustomization);
}

void FLevelStreamAssistantSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	/// get settings object from detail view
	TArray<TWeakObjectPtr<UObject>> OutSettings;
	DetailLayout.GetObjectsBeingCustomized(OutSettings);
	if (OutSettings.Num() > 0)
	{
		SettingsWeakPtr = Cast<ULevelStreamAssistantSettings>(OutSettings[0]);
	}
	
	ULevelStreamAssistantSettings* Settings = SettingsWeakPtr.Get();
	if (!Settings) return;

	ListLevels = TArray<TWeakObjectPtr<ULevelStreaming>>(GetStreamingLevels());

	IDetailCategoryBuilder& CategoryBuilder = DetailLayout.EditCategory("Generate Zones");

	// re-order settings properties.
	CategoryBuilder.AddProperty(GET_MEMBER_NAME_CHECKED(ULevelStreamAssistantSettings, bAutoBind));
	CategoryBuilder.AddProperty(GET_MEMBER_NAME_CHECKED(ULevelStreamAssistantSettings, bBindNearbyZones));
	CategoryBuilder.AddProperty(GET_MEMBER_NAME_CHECKED(ULevelStreamAssistantSettings, StreamingUsage));
	CategoryBuilder.AddProperty(GET_MEMBER_NAME_CHECKED(ULevelStreamAssistantSettings, bIgnoreStaticMeshBoundsExtension));
	CategoryBuilder.AddProperty(GET_MEMBER_NAME_CHECKED(ULevelStreamAssistantSettings, PositiveBoundsExtension));
	CategoryBuilder.AddProperty(GET_MEMBER_NAME_CHECKED(ULevelStreamAssistantSettings, NegativeBoundsExtension));

	// customize ZoneGenerationMode property
	TSharedPtr<IPropertyHandle> ZoneGenerationModePropertyHandle =
		CategoryBuilder.AddProperty(GET_MEMBER_NAME_CHECKED(ULevelStreamAssistantSettings, ZoneGenerationMode)).GetPropertyHandle();
	if (ZoneGenerationModePropertyHandle.IsValid() && ZoneGenerationModePropertyHandle->IsValidHandle())
	{
		ZoneGenerationModePropertyHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FLevelStreamAssistantSettingsCustomization::OnValueChanged_ZoneGenerationMode));
	}

	if (Settings->ZoneGenerationMode == EZoneGenerationMode::EFromSelectedLevels)
	{
		// list view of streaming levels to select.
		CategoryBuilder.AddCustomRow(LOCTEXT("LabelLevelSelection", "Level Selection"))
		.WholeRowContent()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(SearchBoxLevels, SSimpleSearchBox<TWeakObjectPtr<ULevelStreaming>>)
				.ItemToStringArray_Static(&LevelStreamAssistantUtils::LevelToStringArray)
				.OnSearchChanged(this, &FLevelStreamAssistantSettingsCustomization::OnSearchChanged_Level)
			]
			+ SVerticalBox::Slot()
			.MaxHeight(150.0f)
			[
				// todo: use SSplitter for resizable list view in details view
				SAssignNew(ListViewLevels, SListView<TWeakObjectPtr<ULevelStreaming>>)
				.ListItemsSource(&ListLevels)
				.SelectionMode(ESelectionMode::Multi)
				.OnGenerateRow(this, &FLevelStreamAssistantSettingsCustomization::OnGenerateRow_Level)
			]
		];

		// initially select all items
		//ListViewLevels->SetItemSelection(ListLevels, true);
	}

	TSharedPtr<SButton> AutoGenerateZonesButton;

	// button to generate zones.
	CategoryBuilder.AddCustomRow(LOCTEXT("LabelGenerateZones", "Generate Zone(s)"))
	.WholeRowContent()
	[
		SAssignNew(AutoGenerateZonesButton, SButton)
		.HAlign(HAlign_Center)
		.OnClicked(this, &FLevelStreamAssistantSettingsCustomization::OnClicked_GenerateZones)
		.ButtonStyle(FEditorStyle::Get(), "ToggleButton")
		.Text(LOCTEXT("ButtonAutoGenerateZones", "Auto Generate Zones"))
	];

	AutoGenerateZonesButton->SetForegroundColor(FLevelStreamAssistantStyle::GetButtonForegroundAttribute(AutoGenerateZonesButton));
}

void FLevelStreamAssistantSettingsCustomization::CustomizeDetails(const TSharedPtr<IDetailLayoutBuilder>& DetailBuilder)
{
	DetailBuilderWeakPtr = DetailBuilder;
	CustomizeDetails(*DetailBuilder);
}

FReply FLevelStreamAssistantSettingsCustomization::OnClicked_GenerateZones()
{
	if (!GEditor) return FReply::Handled();

	ULevelStreamAssistantSettings* Settings = SettingsWeakPtr.Get();
	if (!Settings) return FReply::Handled();

	switch(Settings->ZoneGenerationMode)
	{
	case EZoneGenerationMode::EFromSelectedLevels:
	{
		if (!ListViewLevels.IsValid()) return FReply::Handled();

		/// get streaming levels from list view selection
		FScopedTransaction Transaction(LOCTEXT("GenerateZonesFromSelectedLevelsTransaction", "Generate Zones from selected Levels"));
		GenerateZonesFromLevels(ListViewLevels->GetSelectedItems());
		break;
	}

	case EZoneGenerationMode::EFromVisibleLevels:
	{
		/// get streaming levels from visible ones in editor.
		FScopedTransaction Transaction(LOCTEXT("GenerateZonesFromVisibleLevelsTransaction", "Generate Zones from visible Levels"));
		TArray<ULevelStreaming*> StreamingLevels = GetStreamingLevels();
		TArray<TWeakObjectPtr<ULevelStreaming>> VisibleStreamingLevels;
		VisibleStreamingLevels.Reserve(StreamingLevels.Num());
		for (ULevelStreaming* LevelStreaming : StreamingLevels)
		{
			if (LevelStreaming && LevelStreaming->GetShouldBeVisibleInEditor())
			{
				VisibleStreamingLevels.Add(LevelStreaming);
			}
		}
		GenerateZonesFromLevels(VisibleStreamingLevels);
		break;
	}

	case EZoneGenerationMode::EFromSelectedActors:
	{
		/// generate zone from actors
		FScopedTransaction Transaction(LOCTEXT("GenerateZoneFromSelectedActorsTransaction", "Generate Zone from selected Actors"));
		if (USelection* ActorSelection = GEditor->GetSelectedActors())
		{
			TArray<AActor*> SelectedActors;
			ActorSelection->GetSelectedObjects<AActor>(SelectedActors);
			GenerateZoneFromActors(SelectedActors);
		}
		break;
	}

	default: return FReply::Handled(); // unknown mode
	}

	return FReply::Handled();
}

void FLevelStreamAssistantSettingsCustomization::OnValueChanged_ZoneGenerationMode()
{
	if (IDetailLayoutBuilder* DetailBuilder = DetailBuilderWeakPtr.Pin().Get())
	{
		DetailBuilder->ForceRefreshDetails();
	}
}

TSharedRef<ITableRow> FLevelStreamAssistantSettingsCustomization::OnGenerateRow_Level(TWeakObjectPtr<ULevelStreaming> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (ULevelStreaming* LevelStreaming = InItem.Get())
	{
		if (SearchBoxLevels.IsValid() && SearchBoxLevels->PassesFilter(InItem))
		{
			return SNew(STableRow<TWeakObjectPtr<ULevelStreaming>>, OwnerTable)
			[
				SNew(STextBlock).Text(FText::FromString(LevelStreamAssistantUtils::GetLevelStreamingName(LevelStreaming)))
			];
		}
	}
	return SNew(STableRow<TWeakObjectPtr<ULevelStreaming>>, OwnerTable);
}

void FLevelStreamAssistantSettingsCustomization::OnSearchChanged_Level()
{
	if (ListViewLevels.IsValid()) ListViewLevels->RebuildList();
}

void FLevelStreamAssistantSettingsCustomization::GenerateZonesFromLevels(const TArray<TWeakObjectPtr<ULevelStreaming>>& SelectedStreamingLevels)
{
	if (!GEditor) return;

	UWorld* World = LevelStreamAssistantUtils::GetWorldFromEditor();
	if (!World) return;

	ULevel* PersistentLevel = World->PersistentLevel;
	if (!PersistentLevel) return;

	ULevelStreamAssistantSettings* Settings = SettingsWeakPtr.Get();
	if (!Settings) return;

	TArray<ALevelStreamingVolume*> GeneratedZones;
	GeneratedZones.Reserve(SelectedStreamingLevels.Num());

	for (ULevelStreaming* LevelStreaming : TWeakArraySafeRange<ULevelStreaming>(SelectedStreamingLevels))
	{
		/// skip levels that are not loaded.
		ULevel* Level = LevelStreaming->GetLoadedLevel();
		if (!Level) continue;

		// todo: calculating bounds of invisible levels may not work properly. temporarily make levels visible.

		// level bounds is used for zone bounds
		FBox LevelBounds = LevelStreamAssistantUtils::CalculateLevelBounds(Level, Settings->bIgnoreStaticMeshBoundsExtension);

		// give a meaningful name
		FString ZoneName = FString::Printf(TEXT("LSV_%s"), *LevelStreamAssistantUtils::GetLevelStreamingName(LevelStreaming));

		if (ALevelStreamingVolume* Zone = GenerateZone(LevelBounds, ZoneName))
		{
			GeneratedZones.Add(Zone);

			if (Settings->bAutoBind)
			{
				// Initially each streaming level have it's own zone.
				LevelStreaming->Modify();
				LevelStreaming->EditorStreamingVolumes.Add(Zone);
			}
		}
	}

	// bind nearby zones after all zones are generated.
	if (Settings->bAutoBind && Settings->bBindNearbyZones)
	{
		for (ULevelStreaming* LevelStreaming : TWeakArraySafeRange<ULevelStreaming>(SelectedStreamingLevels))
		{
			ULevel* Level = LevelStreaming->GetLoadedLevel();
			if (!Level) continue;

			FBox LevelBounds = LevelStreamAssistantUtils::CalculateLevelBounds(Level, Settings->bIgnoreStaticMeshBoundsExtension);

			for (ALevelStreamingVolume* Zone : GeneratedZones)
			{
				if (!Zone) continue;

				// bind overlapping zones to streaming levels.
				if (LevelBounds.Intersect(Zone->GetBounds().GetBox()))
				{
					if (!LevelStreaming->EditorStreamingVolumes.Contains(Zone))
					{
						LevelStreaming->Modify();
						LevelStreaming->EditorStreamingVolumes.Add(Zone);
					}
				}
			}
		}
	}

	/// select generated zones in editor.
	/// note selection change on SelectNone in order to correctly refresh details panel.
	GEditor->SelectNone(/*bNoteSelectionChange*/true, /*bDeselectBSPSurfs*/true);
	for (ALevelStreamingVolume* Zone : GeneratedZones)
	{
		GEditor->SelectActor(Zone, /*bInSelected*/true, /*bNoteSelectionChange*/false);
	}
	GEditor->NoteSelectionChange();
}

void FLevelStreamAssistantSettingsCustomization::GenerateZoneFromActors(const TArray<AActor*>& SelectedActors)
{
	if (!GEditor) return;

	ULevelStreamAssistantSettings* Settings = SettingsWeakPtr.Get();
	if (!Settings) return;

	if (SelectedActors.Num() <= 0) return;

	FBox ActorsBounds = LevelStreamAssistantUtils::CalculateBoundsFromActors(SelectedActors, Settings->bIgnoreStaticMeshBoundsExtension);
	FString ZoneName = TEXT("LSV_Actors");
	if (ALevelStreamingVolume* Zone = GenerateZone(ActorsBounds, ZoneName))
	{
		/// select generated zone in editor.
		/// note selection change on SelectNone in order to correctly refresh details panel.
		GEditor->SelectNone(/*bNoteSelectionChange*/true, /*bDeselectBSPSurfs*/true);
		GEditor->SelectActor(Zone, /*bInSelected*/true, /*bNoteSelectionChange*/true);
	}
}

ALevelStreamingVolume* FLevelStreamAssistantSettingsCustomization::GenerateZone(const FBox& Bounds, const FString& Name)
{
	if (!GEditor) return nullptr;

	UWorld* World = LevelStreamAssistantUtils::GetWorldFromEditor();
	if (!World) return nullptr;

	ULevel* PersistentLevel = World->PersistentLevel;
	if (!PersistentLevel) return nullptr;

	ULevelStreamAssistantSettings* Settings = SettingsWeakPtr.Get();
	if (!Settings) return nullptr;

	// size can not be negative.
	FVector ZoneSize = (Bounds.GetSize()
		+ Settings->PositiveBoundsExtension
		+ Settings->NegativeBoundsExtension).GetAbs();

	FVector ZoneCenter = Bounds.GetCenter()
		+ Settings->PositiveBoundsExtension * 0.5f
		- Settings->NegativeBoundsExtension * 0.5f;

	// create zones
	FTransform Transform(FRotator::ZeroRotator, ZoneCenter);

	///todo: optimize: AddActor will call SelectNone and SelectActor. this is not good for loops.
	ALevelStreamingVolume* Zone = Cast<ALevelStreamingVolume>(GEditor->AddActor(PersistentLevel, ALevelStreamingVolume::StaticClass(), Transform, true));
	if (!Zone) return nullptr;

	// give a unique id
	FString ZoneId = *MakeUniqueObjectName(Zone->GetOuter(), Zone->GetClass(), *Name).ToString();
	Zone->Rename(*ZoneId);
	Zone->SetActorLabel(Name);

	Zone->StreamingUsage = Settings->StreamingUsage;

	// Make cube shaped zones
	UCubeBuilder* CubeBuilder = NewObject<UCubeBuilder>();
	CubeBuilder->X = ZoneSize.X;
	CubeBuilder->Y = ZoneSize.Y;
	CubeBuilder->Z = ZoneSize.Z;
	LevelStreamAssistantUtils::CreateBrushForVolumeActor(Zone, CubeBuilder);

	return Zone;
}

TArray<ULevelStreaming*> FLevelStreamAssistantSettingsCustomization::GetStreamingLevels() const
{
	if (UWorld* World = LevelStreamAssistantUtils::GetWorldFromEditor())
	{
		return World->GetStreamingLevels();
	}
	return TArray<ULevelStreaming*>();
}

#undef LOCTEXT_NAMESPACE