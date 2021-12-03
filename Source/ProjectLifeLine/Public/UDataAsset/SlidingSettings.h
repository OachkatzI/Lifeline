// Project LifeLine

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SlidingSettings.generated.h"


UCLASS()
class PROJECTLIFELINE_API USlidingSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, Category = "Sliding")
	float SlidingForce;

	UPROPERTY(EditAnywhere, Category = "Sliding")
	float MinSlidingSpeed;

	UPROPERTY(EditAnywhere, Category = "Sliding")
	float MaxSlidingSpeed;

	UPROPERTY(EditAnywhere, Category = "Sliding")
	float SlidingGroundFriction;

	UPROPERTY(EditAnywhere, Category = "Sliding")
	float SlidingBrakingDeceleration;

	// How close can you slide towards objects before standing up
	UPROPERTY(EditAnywhere, Category = "Sliding")
	float DistanceToObjectThreshold;

	// Triggers after sliding ends
	UPROPERTY(EditAnywhere, Category = "Sliding")
	float SlidingCooldown;

	// 0 degrees equals running forward, 90 degrees is running sideways
	UPROPERTY(EditAnywhere, Category = "Sliding", meta = (UIMin = "0.0", UIMax = "90.0"))
	float MaxAllowedStrafingAngle;
};
