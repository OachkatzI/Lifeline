// Project LifeLine

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LandingSettings.generated.h"


UCLASS()
class PROJECTLIFELINE_API ULandingSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	
	// Can perform landing only if fallen certain height / vertical distance
	UPROPERTY(EditAnywhere, Category = "Landing")
	bool bUseHeightRequirement;

	// In centimeters
	UPROPERTY(EditAnywhere, Category = "Landing", meta = (EditCondition = "bUseHeightRequirement"))
	float HeightRequirement;

	// Can perform landing only if fallen certain duration
	UPROPERTY(EditAnywhere, Category = "Landing")
	bool bUseTimeRequirement;

	// In seconds
	UPROPERTY(EditAnywhere, Category = "Landing", meta = (EditCondition = "bUseTimeRequirement"))
	float TimeRequirement;

	// In centimeters - The distance to the ground in centimeters for scoring perfect landing when activated
	UPROPERTY(EditAnywhere, Category = "Landing")
	float PerfectLandingGroundDistance;

	// In centimeters, +- the perfect landing activation distance to ground
	UPROPERTY(EditAnywhere, Category = "Landing")
	float PerfectLandingThreshold;

	// In seconds - Cooldown after landing ATTEMPT, also triggers if failed to prevent landing spamming
	UPROPERTY(EditAnywhere, Category = "Landing")
	float LandingCooldown;

	// In centimeters - Unable to perform landing after falling this deep, death on impact
	UPROPERTY(EditAnywhere, Category = "Landing")
	float FallingHeightDeathThreshold;

	UPROPERTY(EditAnywhere, Category = "Landing")
	UAnimMontage* HardLandingMontage;

	UPROPERTY(EditAnywhere, Category = "Landing")
	UAnimMontage* PerfectLandingMontage;

	UPROPERTY(EditAnywhere, Category = "Landing")
	UCameraAnim* PerfectLandingAnim;

};
