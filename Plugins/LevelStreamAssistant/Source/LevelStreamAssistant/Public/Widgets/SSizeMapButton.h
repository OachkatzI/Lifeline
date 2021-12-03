// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class STextBlock;
class FReply;
struct FAssetData;
struct FAssetIdentifier;
struct FPrimaryAssetId;

/**
* This widget displays an overview disk size and memory size of assets and will summon SizeMap window when clicked.
* IAssetManagerEditorModule is not thread safe so in order to keep editor responsive while calculating the size of large assets, 
* the calculation time is spread over multiple ticks. 
*/
class SSizeMapButton : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_RetVal(TArray<FAssetData>, FGetAssetDataList);

	SLATE_BEGIN_ARGS(SSizeMapButton)
	{}
		SLATE_ATTRIBUTE(bool, IsOverviewSizeDisplayed)
		SLATE_EVENT(FGetAssetDataList, GetAssetDataList)
		SLATE_ATTRIBUTE(FText, ToolTipText)
	SLATE_END_ARGS()

	SSizeMapButton();

	void Construct(const FArguments& InArgs);

	// Recalculate overview size of assets.
	void UpdateOverviewSize();

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
	EVisibility GetThrobberVisibility() const;
	int32 GetWidgetIndex() const;
	FReply OnClicked_ShowSizeMap();

	// calculate size of asset data in AssetDataQueue for the given duration. remaining asset data will be left for the next tick.
	void CalculateSizeFor(const float Duration);

	// refresh displayed disk and memory size.
	void Refresh();

	// get asset identifiers from asset data list.
	TArray<FAssetIdentifier> GetAssetIdentifiers() const;

	void GatherDependentAssetData(const TArray<FAssetIdentifier>& AssetIdentifiers, TSet<FAssetIdentifier>& VisitedAssetIdentifiers, const FPrimaryAssetId& FilterPrimaryAsset, int32& NumAssetsWhichFailedToLoad);

	// Returns a user friendly text from the given size in bytes.
	FString MakeBestSizeString(const SIZE_T SizeInBytes, const bool bHasKnownSize) const;

	uint8 bIsCalculating : 1;
	uint8 bContainsUnknownDiskSize : 1;
	uint8 bContainsUnknownMemorySize : 1;

	SIZE_T CalculatedDiskSize = 0;
	SIZE_T CalculatedMemorySize = 0;

	TArray<FAssetData> AssetDataQueue; // this queue is processed inside the tick.

	TSharedPtr<STextBlock> TextBlock_DiskSize;
	TSharedPtr<STextBlock> TextBlock_MemorySize;

	TAttribute<bool> IsOverviewSizeDisplayed;
	FGetAssetDataList GetAssetDataList;
};
