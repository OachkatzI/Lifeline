// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "EditorMode/LevelStreamAssistantSettings.h"

ULevelStreamAssistantSettings::ULevelStreamAssistantSettings()
{
	bAutoBind = true;
	bBindNearbyZones = true;
	StreamingUsage = EStreamingVolumeUsage::SVB_LoadingAndVisibility;
	ZoneGenerationMode = EZoneGenerationMode::EFromVisibleLevels;
	bIgnoreStaticMeshBoundsExtension = true;
	PositiveBoundsExtension = FVector(10.0f, 10.0f, -10.0f);
	NegativeBoundsExtension = FVector(10.0f, 10.0f, -10.0f);
}
