// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "LevelStreamingTriggerFactory.h"
#include "LevelStreamingTrigger.h"

ULevelStreamingTriggerFactory::ULevelStreamingTriggerFactory()
{
	NewActorClass = ALevelStreamingTrigger::StaticClass();
	SpawnPositionOffset = FVector(32.f * 4.f, 0.f, 0.f);
	bUseSurfaceOrientation = true;
}
