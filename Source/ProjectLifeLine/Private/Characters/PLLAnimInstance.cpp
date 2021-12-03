// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PLLAnimInstance.h"
#include "Characters/PLLCharacter.h"
#include "Characters/Movement/PLLCharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PlayerControllers/PLLPlayerController.h"
#include "SceneComponent/PLLRopeShooterComponent.h"
#include "Extension/FVectorExt.h"


void UPLLAnimInstance::NativeInitializeAnimation()
{
	SetRootMotionMode(ERootMotionMode::RootMotionFromEverything);
}


void UPLLAnimInstance::NativeBeginPlay()
{
	KaiCharacter = Cast<APLLCharacter>(TryGetPawnOwner());
	check(KaiCharacter);

	KaiMovementComp = KaiCharacter->GetPLLMovementComp();
	check(KaiMovementComp);

	KaiMovementComp->OnPerformLanding.AddDynamic(this, &UPLLAnimInstance::HandleLandingAnimation);

	KaiController = KaiCharacter->GetPLLController();
	check(KaiController);

	KaiCharacter->GetRopeShooterComp()->OnRopeReady.AddDynamic(this, &UPLLAnimInstance::SetRope);

	check(LandingSettings);
}


void UPLLAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!KaiCharacter || !KaiMovementComp)
	{
		return;
	}

	SetIsInAir();

	SetIsAccelerating();

	SetMovingDirection();

	SetAimOffsetRotation();

	SetSpeed();

	bCanCrouch = KaiMovementComp->bWantsToCrouch;

	FallingHeight = KaiMovementComp->GetCurrentFallingHeight();

	if (Rope)
	{
		bRopeIsAttached = Rope->GetIsAttached();
		bIsSwinging = KaiMovementComp->IsInMovementState(RopeSwinging);
	}
}


void UPLLAnimInstance::SetIsInAir()
{
	bIsInAir = KaiMovementComp->IsFalling();
}


void UPLLAnimInstance::SetMovingDirection()
{
	const auto Rotation = KaiController->AccumulatedDirectionalInput.ToOrientationRotator();
	Direction           = Rotation.Yaw;
}


void UPLLAnimInstance::SetIsAccelerating()
{
	const bool bWasAccelerating = bIsAccelerating;

	FVector OutDir;
	float   OutLength;
	KaiController->AccumulatedDirectionalInput.ToDirectionAndLength(OutDir, OutLength);

	bIsAccelerating = !FMath::IsNearlyZero(OutLength);

	bMovementInputChanged = bIsAccelerating != bWasAccelerating;
}


void UPLLAnimInstance::SetAimOffsetRotation()
{
	AimOffsetRotation = KaiCharacter->CameraLocalRotation;
	
	if (Rope && bRopeIsAttached)
	{
		const FVector DirectionToAnchor = (Rope->GetCurrentSwingPoint() - KaiCharacter->GetRopeShooterComp()->GetComponentLocation()).GetSafeNormal();
	
		RopeAimOffsetAngleForward = FVectorExt::AngleBetween(KaiCharacter->GetActorForwardVector(), DirectionToAnchor) - 90.f;
		RopeAimOffsetAngleForward *= -1;		

		RopeAimOffsetAngleRight = FVectorExt::AngleBetween(KaiCharacter->GetActorRightVector(), DirectionToAnchor) - 90;
	}
}


void UPLLAnimInstance::SetSpeed()
{
	Speed = KaiMovementComp->GetCurrentSpeed();
}


void UPLLAnimInstance::SetRope(APLLRope* NewRope)
{
	Rope = NewRope;
}


void UPLLAnimInstance::HandleLandingAnimation(bool bWasPerfect, float LandingScore)
{
	if (bWasPerfect)
	{
		Montage_Play(LandingSettings->PerfectLandingMontage);
	}
	else
	{
		Montage_Play(LandingSettings->HardLandingMontage);
	}
}
