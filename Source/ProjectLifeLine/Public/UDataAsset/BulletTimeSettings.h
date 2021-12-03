// Project LifeLine

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Curves/CurveFloat.h"
#include "BulletTimeSettings.generated.h"


UCLASS()
class PROJECTLIFELINE_API UBulletTimeSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	
	// Not considering Start and End times
	UPROPERTY(EditDefaultsOnly, Category = "Time Behaviour")
	float ActualBulletTimeDuration;

	UPROPERTY(EditDefaultsOnly, Category = "Time Behaviour")
	float BulletTimeStartDuration;

	UPROPERTY(EditDefaultsOnly, Category = "Time Behaviour")
	float BulletTimeEndDuration;

	// 1 means no dilation, 0 is time freeze
	UPROPERTY(EditAnywhere, Category = "Time Behaviour", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float TotalTimeDilation;

	// Cooldown starts after completion
	UPROPERTY(EditAnywhere, Category = "Time Behaviour", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float BulletTimeCooldown;

	// Must range from (0|0) to (1|1)
	UPROPERTY(EditDefaultsOnly, Category = "Time Behaviour")
	UCurveFloat* BulletTimeStartCurve;

	// Must range from (0|0) to (1|1)
	UPROPERTY(EditDefaultsOnly, Category = "Time Behaviour")
	UCurveFloat* BulletTimeEndCurve;
};
