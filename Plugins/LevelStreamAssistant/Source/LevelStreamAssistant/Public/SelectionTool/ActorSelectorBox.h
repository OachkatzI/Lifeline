// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <GameFramework/Actor.h>
#include <Components/BoxComponent.h>
#include "ActorFactories/ActorFactory.h"
#include "ActorSelectorBox.generated.h"

// A Box that gives you the ability to select other actors inside this volume. This actor is NOT saved in your map and is excluded from gameplay.
UCLASS(Transient, HideCategories = ("Navigation", "Cooking", "Collision", "Tags", "Replication", "LOD", "Rendering", "Input"))
class AActorSelectorBox : public AActor
{
	GENERATED_BODY()

public:
	AActorSelectorBox();

	virtual bool IsComponentRelevantForNavigation(UActorComponent* Component) const override { return false; }

	// Value of 1 will select actors that are fully inside this volume. value of 0 will select all overlapping actors (even slightly overlapped actors).
	UPROPERTY(EditInstanceOnly, Category = "Tools", meta = (ClampMin = "0", ClampMax = "1"))
	float SelectOverlapThreshold = 0.7f;

	UPROPERTY(EditInstanceOnly, Category = "Tools")
	bool bOnlySelectActorsWithMesh = true;

	UPROPERTY(EditInstanceOnly, Category = "Tools")
	bool bExcludeActorsLargerThanThisVolume = true;

	UPROPERTY(EditInstanceOnly, AdvancedDisplay, Category = "Tools")
	bool bSelectHiddenActors = false;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UActorSelectorBoxComponent* BoxComponent = nullptr;
};

UCLASS(Transient, HideCategories = ("Navigation", "Physics", "Collision", "Rendering", "Cooking", "Tags", "HLOD", "Mobile", "AssetUserData"))
class UActorSelectorBoxComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	UActorSelectorBoxComponent();
};

UCLASS()
class UActorSelectorBoxFactory : public UActorFactory
{
	GENERATED_BODY()

public:
	UActorSelectorBoxFactory();
};
