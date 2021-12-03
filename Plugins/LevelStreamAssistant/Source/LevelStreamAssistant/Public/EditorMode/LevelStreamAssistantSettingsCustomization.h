// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class IDetailLayoutBuilder;
class FReply; 
class ITableRow;
class STableViewBase;
template<typename ItemType> class SListView;
template<typename ItemType> class SSimpleSearchBox;

class ULevelStreamAssistantSettings;

class ULevelStreaming;
class AActor;
class ALevelStreamingVolume;

/**
 * Detail customization for auto generating zones.
 */
class FLevelStreamAssistantSettingsCustomization : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

	virtual void CustomizeDetails(const TSharedPtr<IDetailLayoutBuilder>& DetailBuilder) override;

private:
	FReply OnClicked_GenerateZones();

	void OnValueChanged_ZoneGenerationMode();

	TSharedRef<ITableRow> OnGenerateRow_Level(TWeakObjectPtr<ULevelStreaming> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	void OnSearchChanged_Level();

	void GenerateZonesFromLevels(const TArray<TWeakObjectPtr<ULevelStreaming>>& SelectedStreamingLevels);
	void GenerateZoneFromActors(const TArray<AActor*>& SelectedActors);
	ALevelStreamingVolume* GenerateZone(const FBox& Bounds, const FString& Name);

	TArray<ULevelStreaming*> GetStreamingLevels() const;

	TWeakPtr<IDetailLayoutBuilder> DetailBuilderWeakPtr;
	TWeakObjectPtr<ULevelStreamAssistantSettings> SettingsWeakPtr;

	TSharedPtr<SListView<TWeakObjectPtr<ULevelStreaming>>> ListViewLevels;
	TSharedPtr<SSimpleSearchBox<TWeakObjectPtr<ULevelStreaming>>> SearchBoxLevels;

	/// todo update this source when streaming levels are added/removed
	TArray<TWeakObjectPtr<ULevelStreaming>> ListLevels;
};
