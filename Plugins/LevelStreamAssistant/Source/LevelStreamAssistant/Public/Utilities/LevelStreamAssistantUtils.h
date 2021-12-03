// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UWorld;
class ULevel;
class ULevelStreaming;
class AActor;
class AVolume;
class UBrushBuilder;
class FLevelStreamAssistantEdMode;

/**
 * set of helper functions used across level stream assistant plugin.
 */
namespace LevelStreamAssistantUtils
{
	FLevelStreamAssistantEdMode* GetPluginMode();

	UWorld* GetWorldFromEditor();

	TArray<ULevelStreaming*> GetStreamingLevelsFromEditorWorld();

	// @return User friendly name of a LevelStreaming.
	FString GetLevelStreamingName(const ULevelStreaming* LevelStreaming);

	// @return true if LevelA name is lexicographically <= the LevelB name, otherwise false
	bool CompareLevelNames(const TWeakObjectPtr<ULevelStreaming>& LevelA, const TWeakObjectPtr<ULevelStreaming>& LevelB, bool bAscending);

	// @return true if ActorA name is lexicographically <= the ActorB name, otherwise false
	bool CompareActorNames(const TWeakObjectPtr<AActor>& ActorA, const TWeakObjectPtr<AActor>& ActorB, bool bAscending);

	// @return true if NameA name is lexicographically <= the NameB name, otherwise false
	bool CompareNames(const FString& NameA, const FString& NameB, bool bAscending);

	// @return part of the path that is common to all given paths.
	FName GetCommonPath(const TArray<FName>& Paths);

	// used by filters to search for levels by their name.
	void LevelToStringArray(TWeakObjectPtr<ULevelStreaming> InLevel, TArray<FString>& Array);

	// used by filters to search for actors by their name.
	void ActorToStringArray(TWeakObjectPtr<AActor> InActor, TArray<FString>& Array);

	// Refreshes visibility of levels.
	void FlushLevelStreaming();

	// @return Bounding box of a Level.
	FBox CalculateLevelBounds(ULevel* InLevel, bool bIgnoreStaticMeshBoundsExtension);
	
	// @return Bounding box of multiple Actors.
	FBox CalculateBoundsFromActors(const TArray<AActor*>& Actors, bool bIgnoreStaticMeshBoundsExtension);
	
	// @return Bounding box of an Actor.
	FBox CalculateActorBounds(const AActor* Actor, bool bIgnoreStaticMeshBoundsExtension);

	FBox CalculateActorLocalBounds(const AActor* Actor, bool bIgnoreStaticMeshBoundsExtension);

	// Start playing in editor from a location.
	void StartPlayFromLocation(const FVector& StartLocation);

	// Initialize a volume using a brush builder.
	void CreateBrushForVolumeActor(AVolume* Volume, UBrushBuilder* BrushBuilder);
};
