// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "UDataAsset/PLLHitResult.h"
#include "UDataAsset/LandingSettings.h"
#include "PLLAnimInstance.generated.h"


class APLLPlayerController;
class APLLCharacter;
class UPLLCharacterMovementComponent;
class UPLLParkourSystemComponent;
class APLLRope;


UCLASS()
class PROJECTLIFELINE_API UPLLAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	void NativeBeginPlay() override;

	void NativeInitializeAnimation() override;

	void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly)
	bool bCanHang;

	UPROPERTY(BlueprintReadOnly)
	bool bCanEnterWallClimb;

	UPROPERTY(BlueprintReadOnly)
	bool bCanEnterWallRunLeft;

	UPROPERTY(BlueprintReadOnly)
	bool bCanEnterWallRunRight;

	UPROPERTY(BlueprintReadOnly)
	bool bDidEnterWallRunFromGround;

	UPROPERTY(BlueprintReadOnly)
	bool bCanCrouch;

	UPROPERTY(BlueprintReadWrite)
	float HeadAimOffsetWeight = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UPLLHitResult* FloorTracer;

protected:


	UPROPERTY(BlueprintReadOnly)
	APLLPlayerController* KaiController;

	UPROPERTY(BlueprintReadOnly)
	APLLCharacter* KaiCharacter;

	UPROPERTY(BlueprintReadOnly)
	UPLLCharacterMovementComponent* KaiMovementComp;

	UPROPERTY(BlueprintReadOnly)
	APLLRope* Rope;

	UPROPERTY(BlueprintReadOnly)
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAccelerating;

	UPROPERTY(BlueprintReadWrite)
	FRotator AimOffsetRotation;

	UPROPERTY(BlueprintReadWrite)
	float RopeAimOffsetAngleRight;

	UPROPERTY(BlueprintReadWrite)
	float RopeAimOffsetAngleForward;

	UPROPERTY(BlueprintReadWrite)
	bool bRopeIsAttached;

	UPROPERTY(BlueprintReadWrite)
	bool bIsSwinging;

	UPROPERTY(BlueprintReadOnly)
	float Direction;

	UPROPERTY(BlueprintReadOnly)
	float Speed;

	UPROPERTY(BlueprintReadOnly)
	bool bMovementInputChanged;

	UPROPERTY(BlueprintReadOnly)
	float FallingHeight;

	void SetIsInAir();

	void SetMovingDirection();

	void SetIsAccelerating();

	void SetAimOffsetRotation();

	void SetSpeed();

	UFUNCTION()
	void SetRope(APLLRope* NewRope);

	UPROPERTY(EditAnywhere)
	ULandingSettings* LandingSettings;

	UFUNCTION()
	void HandleLandingAnimation(bool bWasPerfect, float LandingScore);

};
