// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Widgets/SSizeMapButton.h"
#include "LevelStreamAssistantStyle.h"
#include "EditorStyleSet.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Images/SThrobber.h"
#include "AssetManagerEditorModule.h"
#include "Engine/AssetManager.h"
#include "Math/UnitConversion.h"

#define LOCTEXT_NAMESPACE "LevelStreamAssistant.SSizeMapButton"

SSizeMapButton::SSizeMapButton()
{
	bIsCalculating = false;
	bContainsUnknownDiskSize = false;
	bContainsUnknownMemorySize = false;
}

void SSizeMapButton::Construct(const FArguments& InArgs)
{
	if (!IAssetManagerEditorModule::IsAvailable())
	{
		ChildSlot
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock).Text(LOCTEXT("LabelNotAvailable", "SizeMap is not available!"))
			.ToolTipText(LOCTEXT("ToolTipNotAvailable", "Make sure 'AssetManagerEditor' module is enabled."))
		];
		return;
	}

	IsOverviewSizeDisplayed = InArgs._IsOverviewSizeDisplayed;
	GetAssetDataList = InArgs._GetAssetDataList;

	TSharedPtr<SButton> SizeMapButton;
	TSharedPtr<SImage> SizeMapDiskImage;
	TSharedPtr<SImage> SizeMapMemoryImage; 

	ChildSlot
	[
		SAssignNew(SizeMapButton, SButton)
		.ButtonStyle(FEditorStyle::Get(), "ToggleButton")
		.ToolTipText(InArgs._ToolTipText)
		.OnClicked(this, &SSizeMapButton::OnClicked_ShowSizeMap)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			[
				SNew(SWidgetSwitcher)
				.WidgetIndex(this, &SSizeMapButton::GetWidgetIndex)
				+ SWidgetSwitcher::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ButtonShowSizeMap", "Show Size Map"))
				]
				+ SWidgetSwitcher::Slot()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.Padding(2.f, 2.f, 4.f, 2.f)
						.AutoWidth()
						[
							SAssignNew(SizeMapDiskImage, SImage)
							.Image(FLevelStreamAssistantStyle::Get().GetBrush("LSA.Size.Disk"))
						]
						+ SHorizontalBox::Slot()
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("TextDiskSize", "Disk Size "))
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SAssignNew(TextBlock_DiskSize, STextBlock)
						]
					]
					+ SVerticalBox::Slot()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.Padding(2.f, 2.f, 4.f, 2.f)
						.AutoWidth()
						[
							SAssignNew(SizeMapMemoryImage, SImage)
							.Image(FLevelStreamAssistantStyle::Get().GetBrush("LSA.Size.Memory"))
						]
						+ SHorizontalBox::Slot()
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("TextMemorySize", "Memory Size "))
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SAssignNew(TextBlock_MemorySize, STextBlock)
						]
					]
				]
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SThrobber)
				.Visibility(this, &SSizeMapButton::GetThrobberVisibility)
			]
		]
	];

	// black foreground color when SizeMapButton is hovered.
	TAttribute<FSlateColor> SizeMapForegroundColor = FLevelStreamAssistantStyle::GetButtonForegroundAttribute(SizeMapButton);
	SizeMapButton->SetForegroundColor(SizeMapForegroundColor);
	SizeMapDiskImage->SetColorAndOpacity(SizeMapForegroundColor);
	SizeMapMemoryImage->SetColorAndOpacity(SizeMapForegroundColor);
}

void SSizeMapButton::UpdateOverviewSize()
{
	// do nothing when the required plugin is not enabled.
	if (!IAssetManagerEditorModule::IsAvailable()) return;

	// reset parameters for new calculation.
	bIsCalculating = true;
	bContainsUnknownDiskSize = false;
	bContainsUnknownMemorySize = false;
	CalculatedDiskSize = 0;
	CalculatedMemorySize = 0;
	AssetDataQueue.Reset();
	Refresh();

	TArray<FAssetIdentifier> AssetIdentifiers = GetAssetIdentifiers();
	TSet<FAssetIdentifier> VisitedAssetIdentifiers;
	int32 NumAssetsWhichFailedToLoad = 0; // todo: display this value to user?
	GatherDependentAssetData(AssetIdentifiers, VisitedAssetIdentifiers, FPrimaryAssetId(), NumAssetsWhichFailedToLoad);
}

void SSizeMapButton::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	
	if (bIsCalculating)
	{
		// spend maximum of 5 milliseconds to calculate size of remaining assets.
		CalculateSizeFor(0.005f);

		Refresh();

		if (AssetDataQueue.Num() == 0)
		{
			bIsCalculating = false;
		}
	}
}

EVisibility SSizeMapButton::GetThrobberVisibility() const
{
	return bIsCalculating ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed;
}

int32 SSizeMapButton::GetWidgetIndex() const
{
	return IsOverviewSizeDisplayed.Get() ? 1 : 0;
}

FReply SSizeMapButton::OnClicked_ShowSizeMap()
{
	if (IAssetManagerEditorModule::IsAvailable())
	{
		TArray<FAssetIdentifier> AssetIdentifiers = GetAssetIdentifiers();
		if (AssetIdentifiers.Num() > 0)
		{
			IAssetManagerEditorModule::Get().OpenSizeMapUI(AssetIdentifiers);
		}
	}

	return FReply::Handled();
}

void SSizeMapButton::CalculateSizeFor(const float Duration)
{
	if (IAssetManagerEditorModule::IsAvailable())
	{
		IAssetManagerEditorModule& EditorModule = IAssetManagerEditorModule::Get();

		FDateTime StartTime = FDateTime::UtcNow();
		
		while (AssetDataQueue.Num() > 0 && (FDateTime::UtcNow() - StartTime).GetTotalSeconds() < Duration)
		{
			FAssetData AssetData = AssetDataQueue.Pop();

			// todo: load assets with FStreamableManager::RequestAsyncLoad() and AssetData.ToSoftObjectPath();
			
			int64 DiskSize = 0, MemorySize = 0;

			// read disk and memory size
			bool bFoundDiskSize = EditorModule.GetIntegerValueForCustomColumn(AssetData, IAssetManagerEditorModule::DiskSizeName, DiskSize);
			bool bFoundMemorySize = EditorModule.GetIntegerValueForCustomColumn(AssetData, IAssetManagerEditorModule::ResourceSizeName, MemorySize);

			// If we're reading cooked data, this will fail for dependencies that are editor only. This is fine, they will have 0 size
			if (bFoundDiskSize) CalculatedDiskSize += DiskSize;
			else bContainsUnknownDiskSize = true;

			if (bFoundMemorySize) CalculatedMemorySize += MemorySize;
			else bContainsUnknownMemorySize = true;
		}
	}
}

void SSizeMapButton::Refresh()
{
	if (TextBlock_DiskSize.IsValid())
	{
		TextBlock_DiskSize->SetText(FText::FromString(MakeBestSizeString(CalculatedDiskSize, !bContainsUnknownDiskSize)));
	}
	if (TextBlock_MemorySize.IsValid())
	{
		TextBlock_MemorySize->SetText(FText::FromString(MakeBestSizeString(CalculatedMemorySize, !bContainsUnknownMemorySize)));
	}
}

TArray<FAssetIdentifier> SSizeMapButton::GetAssetIdentifiers() const
{
	TArray<FAssetIdentifier> AssetIdentifiers;
	if (GetAssetDataList.IsBound())
	{
		IAssetManagerEditorModule::ExtractAssetIdentifiersFromAssetDataList(GetAssetDataList.Execute(), AssetIdentifiers);
	}
	return AssetIdentifiers;
}

/// Modified from Engine/Plugins/Editor/AssetManagerEditor/Source/AssetManagerEditor/Private/SSizeMap.cpp SSizeMap::GatherDependenciesRecursively
void SSizeMapButton::GatherDependentAssetData(const TArray<FAssetIdentifier>& InAssetIdentifiers, TSet<FAssetIdentifier>& VisitedAssetIdentifiers, const FPrimaryAssetId& FilterPrimaryAsset, int32& NumAssetsWhichFailedToLoad)
{
	IAssetManagerEditorModule& EditorModule = IAssetManagerEditorModule::Get();
	const FAssetManagerEditorRegistrySource* CurrentRegistrySource = EditorModule.GetCurrentRegistrySource(false);

	const FAssetRegistryState* CurrentPlatformState = CurrentRegistrySource ? CurrentRegistrySource->RegistryState : nullptr;
	
	if (!CurrentPlatformState) return;

	for (const FAssetIdentifier AssetIdentifier : InAssetIdentifiers)
	{
		FName AssetPackageName = AssetIdentifier.IsPackage() ? AssetIdentifier.PackageName : NAME_None;
		FString AssetPackageNameString = AssetPackageName != NAME_None ? AssetPackageName.ToString() : FString();
		FPrimaryAssetId AssetPrimaryId = AssetIdentifier.GetPrimaryAssetId();
		int32 ChunkId = UAssetManager::ExtractChunkIdFromPrimaryAssetId(AssetPrimaryId);
		int32 FilterChunkId = UAssetManager::ExtractChunkIdFromPrimaryAssetId(FilterPrimaryAsset);
		const FAssetManagerChunkInfo* FilterChunkInfo = FilterChunkId != INDEX_NONE ? CurrentRegistrySource->ChunkAssignments.Find(FilterChunkId) : nullptr;

		// Only support packages and primary assets
		if (AssetPackageName == NAME_None && !AssetPrimaryId.IsValid())
		{
			continue;
		}

		// Don't bother showing code references
		if (AssetPackageNameString.StartsWith(TEXT("/Script/")))
		{
			continue;
		}

		bool bAlreadyVisited = false;
		VisitedAssetIdentifiers.Add(AssetIdentifier, &bAlreadyVisited);
		if (!bAlreadyVisited)
		{
			FAssetData AssetData;
			if (AssetPackageName != NAME_None)
			{
				AssetData.AssetName = AssetPackageName;
				AssetData.AssetClass = FName(*LOCTEXT("MissingAsset", "MISSING!").ToString());

				const FString AssetPathString = AssetPackageNameString + TEXT(".") + FPackageName::GetLongPackageAssetName(AssetPackageNameString);
				const FAssetData* FoundPointer = CurrentPlatformState->GetAssetByObjectPath(FName(*AssetPathString));

				if (FoundPointer)
				{
					AssetData = *FoundPointer;
				}
			}
			else
			{
				AssetData = IAssetManagerEditorModule::CreateFakeAssetDataFromPrimaryAssetId(AssetPrimaryId);
			}

			bool bHasKnownSize = false;

			if (AssetData.IsValid())
			{
				FAssetManagerDependencyQuery DependencyQuery = FAssetManagerDependencyQuery::None();
				if (AssetPackageName != NAME_None)
				{
					DependencyQuery.Categories = UE::AssetRegistry::EDependencyCategory::Package;
					DependencyQuery.Flags = UE::AssetRegistry::EDependencyQuery::Hard;
				}
				else
				{
					DependencyQuery.Categories = UE::AssetRegistry::EDependencyCategory::Manage;
					DependencyQuery.Flags = UE::AssetRegistry::EDependencyQuery::Direct;
				}
				
				TArray<FAssetIdentifier> References;

				if (ChunkId != INDEX_NONE)
				{
					// Look in the platform state
					const FAssetManagerChunkInfo* FoundChunkInfo = CurrentRegistrySource->ChunkAssignments.Find(ChunkId);
					if (FoundChunkInfo)
					{
						References.Append(FoundChunkInfo->ExplicitAssets.Array());
					}
				}
				else
				{
					CurrentPlatformState->GetDependencies(AssetIdentifier, References, DependencyQuery.Categories, DependencyQuery.Flags);
				}

				// Filter for registry source
				IAssetManagerEditorModule::Get().FilterAssetIdentifiersForCurrentRegistrySource(References, DependencyQuery, true);

				TArray<FAssetIdentifier> ReferencedAssetIdentifiers;

				for (FAssetIdentifier& FoundAssetIdentifier : References)
				{
					if (FoundAssetIdentifier.IsPackage())
					{
						FName FoundPackageName = FoundAssetIdentifier.PackageName;

						if (FoundPackageName != NAME_None)
						{
							if (FilterChunkId != INDEX_NONE)
							{
								if (!FilterChunkInfo || !FilterChunkInfo->AllAssets.Contains(FoundPackageName))
								{
									// Not found in the chunk list, skip
									continue;
								}
							}
							else if (FilterPrimaryAsset.IsValid())
							{
								// Check to see if this is managed by the filter asset
								TArray<FAssetIdentifier> Managers;
								CurrentPlatformState->GetReferencers(FoundAssetIdentifier, Managers, UE::AssetRegistry::EDependencyCategory::Manage);

								if (!Managers.Contains(FilterPrimaryAsset))
								{
									continue;
								}
							}

							ReferencedAssetIdentifiers.Add(FoundPackageName);
						}
					}
					else
					{
						ReferencedAssetIdentifiers.Add(FoundAssetIdentifier);
					}
				}

				if (AssetPackageName != NAME_None)
				{
					// put asset data in the queue, their size will be calculated in tick. 
					AssetDataQueue.Push(AssetData);
					bHasKnownSize = true; // for now assume that size is known.
				}
				else
				{
					// Virtual node, size is known to be 0
					bHasKnownSize = true;
				}

				const FPrimaryAssetId& FilterAsset = ChunkId != INDEX_NONE ? AssetPrimaryId : FilterPrimaryAsset;

				// Now visit all of the assets that we are referencing
				GatherDependentAssetData(ReferencedAssetIdentifiers, VisitedAssetIdentifiers, FilterAsset, NumAssetsWhichFailedToLoad);
			}
			else
			{
				++NumAssetsWhichFailedToLoad;
			}

			if (!bHasKnownSize)
			{
				bContainsUnknownDiskSize = true;
				bContainsUnknownMemorySize = true;
			}
		}
	}
}

/// Copied from Engine/Plugins/Editor/AssetManagerEditor/Source/AssetManagerEditor/Private/SSizeMap.cpp MakeBestSizeString
FString SSizeMapButton::MakeBestSizeString(const SIZE_T SizeInBytes, const bool bHasKnownSize) const
{
	FString BestSizeString;

	const FNumericUnit<double> BestUnit = FUnitConversion::QuantizeUnitsToBestFit((double)SizeInBytes, EUnit::Bytes);

	if (BestUnit.Units == EUnit::Bytes)
	{
		// We ended up with bytes, so show a decimal number
		BestSizeString = FString::Printf(TEXT("%s %s"),
			*FText::AsNumber(static_cast<uint64>(SizeInBytes)).ToString(),
			*LOCTEXT("Bytes", "bytes").ToString());
	}
	else
	{
		// Show a fractional number with the best possible units
		FNumberFormattingOptions NumberFormattingOptions;
		NumberFormattingOptions.MaximumFractionalDigits = 3;
		NumberFormattingOptions.MinimumFractionalDigits = 0;
		NumberFormattingOptions.MinimumIntegralDigits = 1;
		BestSizeString = FString::Printf(TEXT("%s %s"),
			*FText::AsNumber(BestUnit.Value, &NumberFormattingOptions).ToString(),
			FUnitConversion::GetUnitDisplayString(BestUnit.Units));
	}

	if (!bHasKnownSize)
	{
		if (SizeInBytes == 0)
		{
			BestSizeString = LOCTEXT("UnknownSize", "unknown size").ToString();
		}
		else
		{
			BestSizeString = FString::Printf(TEXT("%s %s"),
				*LOCTEXT("UnknownSizeButAtLeastThisBig", "at least").ToString(),
				*BestSizeString);
		}
	}
	return BestSizeString;
}

#undef LOCTEXT_NAMESPACE
