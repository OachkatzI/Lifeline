// Project LifeLine


#include "SceneComponent/PLLRopeShooterComponent.h"
#include "Camera/CameraComponent.h"
#include "Characters/PLLBulletTimeComponent.h"
#include "Characters/PLLCharacter.h"
#include "Characters/Movement/PLLCharacterMovementComponent.h"
#include "Extension/PLLFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UDataAsset/PLLAimAssistantSettings.h"
#include "UDataAsset/PLLRopeSetting.h"


UPLLRopeShooterComponent::UPLLRopeShooterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UPLLRopeShooterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	TickBubbleCursor();
	TickGravity(DeltaTime);

	PlayerController->AddPitchInput(CachedPitch);
	PlayerController->AddYawInput(CachedYaw);
	CachedPitch = 0;
	CachedYaw   = 0;
}


void UPLLRopeShooterComponent::BeginPlay()
{
	Super::BeginPlay();

	KaiCharacter = Cast<APLLCharacter>(GetOwner());

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	Rope              = GetWorld()->SpawnActor<APLLRope>(RopeClass, GetComponentLocation(), GetComponentRotation(), SpawnParams);
	check(Rope);

	Rope->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
	OnRopeReady.Broadcast(Rope);

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	check(PlayerController);
	CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	check(CameraManager);
	check(Settings);

	RemainingGrappleBoostCharges = Rope->GetRopeSettings().MaxGrappleBoostCharges;
}


void UPLLRopeShooterComponent::Fire()
{
	if (!GetOwner() 
		|| KaiCharacter->GetPLLMovementComp()->IsInMovementState(GroundSliding)
		|| KaiCharacter->GetPLLMovementComp()->IsInMovementState(MantleShimmy)
		|| KaiCharacter->GetPLLMovementComp()->IsInMovementState(RopeClimbing))
	{
		return;
	}

	if (CanShootRope)
	{
		if (KaiCharacter->GetPLLMovementComp()->IsInMovementState(WallRunning))
		{
			KaiCharacter->GetPLLMovementComp()->EndWallRunning(false);
		}
		
		float DistanceToHitPoint = FVector::Distance(RopeTargetPosition, Rope->GetActorLocation());

		if (DistanceToHitPoint > Rope->GetRopeSettings().RopeMaxLength)
		{
			return;
		}

		const float NewRopeLength = (DistanceToHitPoint < Rope->GetRopeSettings().RopeMinLength)
			                            ? Rope->GetRopeSettings().RopeMinLength
			                            : DistanceToHitPoint;

		const FRopeAttachmentParams AttachParams = FRopeAttachmentParams(RopeTargetActor, RopeTargetPosition, NewRopeLength);

		Rope->Attach(true, AttachParams);
		KaiCharacter->FindComponentByClass<UPLLBulletTimeComponent>()->EndBulletTime();
	}
	CanShootRope = false;
}


void UPLLRopeShooterComponent::Release()
{
	Rope->Release();
}


void UPLLRopeShooterComponent::TryApplyGrappleBoost()
{
	if (GetRope()->GetIsAttached() && RemainingGrappleBoostCharges > 0)
	{
		const FVector ViewVector   = KaiCharacter->GetFirstPersonCamera()->GetForwardVector();
		const FVector AnchorVector = FVector(Rope->GetCurrentSwingPoint() - GetComponentLocation()).GetSafeNormal();

		const FVector BoostVector = FMath::Lerp(AnchorVector, ViewVector, GetRope()->GetRopeSettings().GrappleBoostViewDirectionWeight);
		Cast<UPLLCharacterMovementComponent>(KaiCharacter->GetMovementComponent())->ApplyGrappleBoost(BoostVector);
		GetRope()->Release();

		RemainingGrappleBoostCharges--;
		OnRemainingGrappleChargesChanged.Broadcast(RemainingGrappleBoostCharges);

		GetWorld()->GetTimerManager().SetTimer(GrappleBoostCooldownHandle, this, &UPLLRopeShooterComponent::OnGrappleCooldownEnd,
		                                       GetRope()->GetRopeSettings().GrappleBoostCooldown);
		OnGrapplyBoostCooldownStart.Broadcast();
	}
}


float UPLLRopeShooterComponent::GetRemainingGrappleBoostCooldown() const
{
	// if -1 is returned, Timer is currently not active
	const float RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(GrappleBoostCooldownHandle);

	return RemainingTime < 0.f ? 0 : RemainingTime;
}


void UPLLRopeShooterComponent::OnGrappleCooldownEnd()
{
	RemainingGrappleBoostCharges = Rope->GetRopeSettings().MaxGrappleBoostCharges;
	OnRemainingGrappleChargesChanged.Broadcast(RemainingGrappleBoostCharges);

	OnGrapplyBoostCooldownEnd.Broadcast();
}


void UPLLRopeShooterComponent::TickBubbleCursor()
{
	if (LineTrace->bDidHit)
	{
		CanShootRope       = true;
		RopeTargetPosition = LineTrace->Hit.Location;
		RopeTargetNormal   = LineTrace->Hit.ImpactNormal;
		RopeTargetActor    = LineTrace->Hit.GetActor();
		return;
	}
	if (BubbleTrace->bDidHit)
	{
		CanShootRope       = true;
		RopeTargetPosition = BubbleTrace->Hit.ImpactPoint;
		RopeTargetNormal   = BubbleTrace->Hit.ImpactNormal;
		RopeTargetActor    = BubbleTrace->Hit.GetActor();
		return;
	}
	CanShootRope = false;
}


void UPLLRopeShooterComponent::TickGravity(float DeltaTime)
{
	//Note: This also enables this type of aim assistant if the controller is only connected allowing for a player to enable this assistant for the mouse.
	if (!UPLLFunctionLibrary::IsGamePadConnected())
		return;

	if (LineTrace->bDidHit || KaiCharacter->GetMovementComponent()->IsMovingOnGround())
	{
		return;
	}
	if (GravityTrace->bDidHit)
	{
		CurrentRotation = CameraManager->GetCameraRotation();

		TargetRotation = UKismetMathLibrary::FindLookAtRotation(CameraManager->GetCameraLocation(), GravityTrace->Hit.ImpactPoint);

		const auto TmpTargetRotation = FMath::RInterpConstantTo(CurrentRotation, TargetRotation, DeltaTime,
		                                                        Settings->GravityRotationLerpSpeed);

		CachedPitch = CurrentRotation.Pitch - TmpTargetRotation.Pitch;
		CachedYaw   = TmpTargetRotation.Yaw - CurrentRotation.Yaw;
	}
}
