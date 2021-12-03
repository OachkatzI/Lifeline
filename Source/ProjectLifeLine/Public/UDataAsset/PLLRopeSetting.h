// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PLLRopeSetting.generated.h"

UCLASS()
class PROJECTLIFELINE_API UPLLRopeSetting : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BoostPower = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxBoost = 40;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RopeMinLength = 150;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RopeMaxLength = 600;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bScaleBoostByViewDirection = true;

	/** How much the player should be pushed towards the SwingPoint if the rope gets to long while swinging */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CounterImpulseStrength = 400;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bBoostAfterRetract = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BoostAfterRetractStrength = 20;

	// In Degrees
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PerfectReleaseAngle = 45;

	// In Degrees
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PerfectReleaseAngleTolerance = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GrappleBoostStrength = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 MaxGrappleBoostCharges = 3;

	// In Seconds
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GrappleBoostCooldown = 6.f;

	// Ratio of ViewDirection to AnchorDirection
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (UIMin = "0.0", UIMax = "1.0"))
	float GrappleBoostViewDirectionWeight = .25f;
};
