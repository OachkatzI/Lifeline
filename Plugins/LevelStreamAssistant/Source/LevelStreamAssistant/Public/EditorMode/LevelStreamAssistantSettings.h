// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LevelStreamAssistantSettings.generated.h"

UENUM()
enum class EZoneGenerationMode : uint8
{
	/** Generate zone for each selected levels. */
	EFromSelectedLevels UMETA(DisplayName = "Generate from selected levels."),

	/** Generate zone for each visible levels. */
	EFromVisibleLevels UMETA(DisplayName = "Generate from visible levels."),

	/** Generate a zone from selected actors in viewport. */
	EFromSelectedActors UMETA(DisplayName = "Generate from selected actors."),
};

/**
 * settings for auto generating zones.
 */
UCLASS(Config=EditorPerProjectUserSettings)
class ULevelStreamAssistantSettings : public UObject
{
	GENERATED_BODY()
	
public:
	ULevelStreamAssistantSettings();

	virtual bool IsEditorOnly() const override final { return true; }

	// Automatically bind each zone to it's level.
	UPROPERTY(EditAnywhere, Config, Category = "Generate Zones", meta = (NoResetToDefault))
	uint8 bAutoBind : 1;

	// Bind nearby zones to each level as well.
	UPROPERTY(EditAnywhere, Config, Category = "Generate Zones", meta = (NoResetToDefault, EditCondition = "bAutoBind"))
	uint8 bBindNearbyZones : 1;

	UPROPERTY(EditAnywhere, Config, Category = "Generate Zones", AdvancedDisplay, meta = (NoResetToDefault))
	uint8 bIgnoreStaticMeshBoundsExtension : 1;

	/** Determines what this volume is used for, e.g. whether to control loading, loading and visibility or just visibilty (blocking on load) */
	UPROPERTY(EditAnywhere, Config, Category = "Generate Zones")
	TEnumAsByte<enum EStreamingVolumeUsage> StreamingUsage;

	UPROPERTY(EditAnywhere, Config, Category = "Generate Zones", meta = (NoResetToDefault))
	EZoneGenerationMode ZoneGenerationMode;

	UPROPERTY(EditAnywhere, Config, Category = "Generate Zones", AdvancedDisplay)
	FVector PositiveBoundsExtension;

	UPROPERTY(EditAnywhere, Config, Category = "Generate Zones", AdvancedDisplay)
	FVector NegativeBoundsExtension;
};
