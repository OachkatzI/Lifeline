// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Movement/PLLCharacterMovementComponent.h"
#include <algorithm>
#include <string>
#include "Characters/PLLAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Actors/PLLRope.h"
#include "UDataAsset/PLLRopeSetting.h"
#include "Characters/PLLCharacter.h"
#include "SceneComponent/PLLCharacterSensorComponent.h"
#include "PLLCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "SceneComponent/PLLRopeShooterComponent.h"
#include "Extension/FVectorExt.h"
#include "Characters/PLLCharacter.h"


static bool             DebugShowMovementStates = true;
FAutoConsoleVariableRef CVARDebugMovementStatePrinting(TEXT("PLL.PrintMovementState"), DebugShowMovementStates,
                                                       TEXT("Print current MovementState to screen"), ECVF_Cheat);

static bool             DebugShowParkourInfo = false;
FAutoConsoleVariableRef CVARDebugParkourInfoPrinting(TEXT("PLL.PrintParkourInfo"), DebugShowParkourInfo,
                                                     TEXT("Print Parkour info like landing stuff to screen"), ECVF_Cheat);


void UPLLCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	KaiCharacter = Cast<APLLCharacter>(GetCharacterOwner());
	check(KaiCharacter);

	AnimInstance = KaiCharacter->GetPLLAnimInstance();
	check(AnimInstance);

	KaiController = KaiCharacter->GetPLLController();
	check(KaiController);

	CameraManager = KaiController->GetPllCameraManager();
	check(CameraManager);

	Sensor = KaiCharacter->FindComponentByClass<UPLLCharacterSensorComponent>();
	AddTickPrerequisiteComponent(Sensor);

	KaiCharacter->GetRopeShooterComp()->OnRopeReady.AddDynamic(this, &UPLLCharacterMovementComponent::SetRope);

	//Setup default values
	PLLMovementState = None;
	SetPlaneConstraintEnabled(true);
	DefaultMaxWalkSpeed = MaxWalkSpeed;
	LastRotation        = KaiCharacter->GetActorRotation();

	StandingCapsuleHalfHeight  = GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	DefaultGroundFriction      = GroundFriction;
	DefaultBrakingDeceleration = BrakingDecelerationWalking;

	check(SlidingSettings);

	//From Parkour system

	Actor         = GetOwner();
	KaiController = Cast<APLLPlayerController>(GetWorld()->GetFirstPlayerController());
	KaiCharacter  = Cast<APLLCharacter>(Actor);
	Mesh          = KaiCharacter->GetMesh();
	Capsule       = KaiCharacter->GetCapsuleComponent();
	CapsuleHh     = Capsule->GetScaledCapsuleHalfHeight();
	AnimInstance  = Cast<UPLLAnimInstance>(Mesh->GetAnimInstance());

	check(LandingSettings);

	OnStartFalling.AddDynamic(this, &UPLLCharacterMovementComponent::StartListeningForLanding);
	OnStopFalling.AddDynamic(this, &UPLLCharacterMovementComponent::PerformLanding);
	OnStartRopeSwinging.AddDynamic(this, &UPLLCharacterMovementComponent::StopListeningForLanding);
	OnEndRopeSwinging.AddDynamic(this, &UPLLCharacterMovementComponent::StartListeningForLanding);
	KaiCharacter->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &UPLLCharacterMovementComponent::HandleOnCapsuleHit);
}


void UPLLCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CalculateCurrentSpeed();
	Direction = Velocity.GetSafeNormal();

	RotationDelta    = LastRotation - GetLastUpdateRotation();
	LastRotation     = GetLastUpdateRotation();
	bCanStartSliding = CheckCanSlide();


	if (DebugShowMovementStates)
	{
		UKismetSystemLibrary::PrintString(this, FString("[MovementMode] ").Append(GetMovementName()), true, false, FLinearColor::Yellow,
		                                  0.f);
		UKismetSystemLibrary::PrintString(this, FString("[Speed] ").Append(std::to_string(Speed).c_str()), true, false,
		                                  FLinearColor::Yellow, 0.f);
		// UKismetSystemLibrary::PrintString(GetWorld(), FString::SanitizeFloat(MaxWalkSpeed));
	}

	// KaiCharacter->bUseControllerRotationYaw = !IsGroundedIdling();
	switch (PLLMovementState)
	{
	case RopeClimbing:
		break;
	case RopeSwinging:
		TickRopeSwinging(DeltaTime);
		break;
	case MantleShimmy:
		break;
	case WallRunning:
		TickWallRunning(DeltaTime);
		break;
	case GroundSliding:
		TickGroundSliding(DeltaTime);
		break;
	case None:
		break;
	}

	if (MovementMode == MOVE_Walking)
	{
		TickRunning(DeltaTime);

		// Detach rope when walking away from anchor point
		if (Rope->GetIsAttached() && FVector::Distance(GetActorLocation(), Rope->GetCurrentSwingPoint()) >= Rope->GetRopeSettings().
			RopeMaxLength * 1.25f)
		{
			KaiCharacter->GetRopeShooterComp()->GetRope()->Release();
			return;
		}
	}

	if (CustomMovementMode == RopeSwinging && (Sensor->FloorTraceHit.Distance > 0.f && Sensor->FloorTraceHit.Distance <= 5.f))
	{
		KaiCharacter->GetRopeShooterComp()->GetRope()->Release();
		SetMovementState(MOVE_Walking);
	}

	bCanStartCrouching = IsGroundedIdling();

	ResolveMovementPostSliding();


	//From Parkour system
	FrameTime       = DeltaTime;
	bIsInAir        = IsFalling();
	bIsAccelerating = GetCurrentAcceleration().Size() > 0.0f ? true : false;
	// KaiController->bCanJump = WallFeetTracer->bDidHit && WallFeetTracer->Hit.Distance < MaxDistanceFromWallToEnter ? false : true;

	TryEnterHanging();
	TryGetOnCube();
	TickMantleOnObject();
	if (bCanPerformLaunch) ListenToMovementInput();

	if (IsInMovementState(MOVE_Walking))
	{
		StopListeningForLanding();
	}

	// Calculate current landing score if we were to attempt a perfect landing now
	if (bWantsToLand)
	{
		bMetLandingRequirements = CheckLandingRequirements(DeltaTime);

		CalculateCurrentLandingScore();

		if (bWantsToSlide && FloorTracer->Hit.IsValidBlockingHit())
		{
			if (FVectorExt::AngleBetween(FloorTracer->Hit.Normal, KaiCharacter->GetActorUpVector()) > 20.f && FloorTracer->Hit.Distance <=
				50.f)
			{
				StartSliding();
			}
		}
	}

	LastPosition = KaiCharacter->GetActorLocation();
}


//Running
void UPLLCharacterMovementComponent::TickRunning(float DeltaTime)
{
	if (bIsInFlowState)
	{
		MaxWalkSpeed = MaxFlowRunSpeed;
	}
	else
	{
		const float SpeedDif = MaxFlowRunSpeed - DefaultMaxWalkSpeed;
		MaxWalkSpeed += (SpeedDif / TimeToAccelerateToMaxFlow) * DeltaTime;
		MaxWalkSpeed   = FMath::Min(MaxWalkSpeed, MaxFlowRunSpeed); //This line should technically not be needed
		bIsInFlowState = MaxWalkSpeed > MaxFlowRunSpeed - 10;
	}

	if (Speed < DefaultMaxWalkSpeed) { ExitFlowState(); }
	const float RotYawDif = FMath::Abs(GetLastUpdateRotation().Yaw - KaiCharacter->GetActorRotation().Yaw);
	if (FMath::Abs(RotationDelta.Yaw) > 3) { ExitFlowState(); }
	// UKismetSystemLibrary::PrintString(GetWorld(), FString::SanitizeFloat(RotationDelta.Yaw));
	// UKismetSystemLibrary::PrintString(GetWorld(), FString::SanitizeFloat(RotYawDif));
}


void UPLLCharacterMovementComponent::ExitFlowState()
{
	MaxWalkSpeed   = DefaultMaxWalkSpeed;
	bIsInFlowState = false;
}


//RopeSwinging
void UPLLCharacterMovementComponent::SetRope(APLLRope* NewRope)
{
	Rope = NewRope;
	check(Rope);

	Rope->OnRopeAttached.AddDynamic(this, &UPLLCharacterMovementComponent::StartRopeSwinging);
	Rope->OnRopeReleased.AddDynamic(this, &UPLLCharacterMovementComponent::EndRopeSwinging);
}


void UPLLCharacterMovementComponent::StartRopeSwinging()
{
	if (IsFalling())
	{
		SetMovementState(RopeSwinging);
		OnStartRopeSwinging.Broadcast();
	}
}


void UPLLCharacterMovementComponent::TickRopeSwinging(float DeltaTime)
{
	if (DebugShowMovementStates)
	{
		UKismetSystemLibrary::PrintString(this, "[MovementMode] RopeSwinging", true, false, FLinearColor::Yellow, 0.f);
	}

	FVector ForceToSwingPoint;
	{
		// This is what gives us the smooth swing curve.
		// Get the amount of how much the velocity is working relative to the direction to the Swing Point.
		// Multiply this to the normalized direction to the swing point
		// Invert to get force towards swingpoint And scale by a magic number so result is smoother. 2 cause looks fine 
		FVector     DirectionFromPointToPlayer = KaiCharacter->GetActorLocation() - Rope->GetCurrentSwingPoint();
		const float DotBetweenVelDir           = FVector::DotProduct(KaiCharacter->GetVelocity(), DirectionFromPointToPlayer);
		DirectionFromPointToPlayer.Normalize();
		DirectionFromPointToPlayer *= DotBetweenVelDir;
		DirectionFromPointToPlayer *= -2; // Magic Number;

		ForceToSwingPoint = DirectionFromPointToPlayer;
	}

	const bool IsBelowSwingPoint = KaiCharacter->GetActorLocation().Z < Rope->GetCurrentSwingPoint().Z;
	FVector    ForceToApply      = IsBelowSwingPoint ? ForceToSwingPoint : FVector(0);

	// Reduce Force Towards SwingPoint according to Rope Length
	const float ReduceFactorsToRope = FVector::Distance(KaiCharacter->GetActorLocation(), Rope->GetCurrentSwingPoint()) / Rope->
		GetRopeLength();
	ForceToApply * FMath::Clamp(ReduceFactorsToRope, 0.f, 1.f);

	AddForce(ForceToApply);

	// Boost Velocity While Swinging
	FVector SwingBoost;
	{
		auto NormalizedVelocity = KaiCharacter->GetVelocity();
		NormalizedVelocity.Normalize(0.0001);

		float BoostValue = FVector::DotProduct(NormalizedVelocity,
		                                       UKismetMathLibrary::GetForwardVector(CameraManager->GetCameraRotation()));
		BoostValue = FMath::Clamp(BoostValue, 0.f, 1.f);
		BoostValue *= FMath::Clamp(ReduceFactorsToRope, 0.f, 1.f) * Rope->GetRopeSettings().BoostPower;
		BoostValue = Rope->GetRopeSettings().bScaleBoostByViewDirection ? BoostValue : 1.f;

		SwingBoost = NormalizedVelocity * BoostValue;
	}
	RepulsionForce = 4000.f;
	ForceToApply   = IsBelowSwingPoint ? SwingBoost : FVector(0);
	ForceToApply   = UKismetMathLibrary::Vector_ClampSizeMax(ForceToApply, Rope->GetRopeSettings().MaxBoost);

	AddForce(ForceToApply);

	// Add counter-impulse, so that the character won't fall despite the cable length
	{
		const FVector LookAtSwingPointRotation = UKismetMathLibrary::FindLookAtRotation(KaiCharacter->GetActorLocation(),
		                                                                                Rope->GetCurrentSwingPoint()).Vector();
		const float CurrentRopeLengthRest = FVector::Distance(KaiCharacter->GetActorLocation(), Rope->GetCurrentSwingPoint()) - Rope->
			GetRopeLength();
		const float CounterImpulseStrength = FMath::Max(CurrentRopeLengthRest, 0.f) * Rope->GetRopeSettings().CounterImpulseStrength;

		const FVector CounterImpulse = LookAtSwingPointRotation * CounterImpulseStrength;
		AddImpulse(CounterImpulse);
	}
}


void UPLLCharacterMovementComponent::EndRopeSwinging(float ReleaseAngle)
{
	if (FMath::IsNearlyEqual(ReleaseAngle, Rope->GetRopeSettings().PerfectReleaseAngle,
	                         Rope->GetRopeSettings().PerfectReleaseAngleTolerance))
	{
		ApplyPerfectRopeReleaseBoost();
	}
	if (MovementMode != MOVE_Walking)
	{
		SetMovementState(MOVE_Falling);
	}
	OnEndRopeSwinging.Broadcast();
}


void UPLLCharacterMovementComponent::ApplyPerfectRopeReleaseBoost()
{
	FVector NormalizedVelocity = Velocity;
	NormalizedVelocity.Normalize();
	FVector Boost = NormalizedVelocity * Rope->GetRopeSettings().BoostAfterRetractStrength;
	AddImpulse(Boost, true);
}


void UPLLCharacterMovementComponent::ApplyGrappleBoost(FVector BoostDirection)
{
	const FVector LaunchForce = BoostDirection * Rope->GetRopeSettings().GrappleBoostStrength;

	CharacterOwner->LaunchCharacter(LaunchForce, true, true);
}


//Wallrunnning
FVector UPLLCharacterMovementComponent::GetWallRunDirection()
{
	FVector RunDirection = FVector::ZeroVector;
	if (Sensor->bDidLeftTraceHit)
	{
		RunDirection = FVector::CrossProduct(Sensor->LeftTraceHit.Normal, FVector(0, 0, 1));
	}
	else if (Sensor->bDidRightTraceHit)
	{
		RunDirection = FVector::CrossProduct(Sensor->RightTraceHit.Normal, FVector(0, 0, -1));
	}

	return RunDirection;
}


void UPLLCharacterMovementComponent::TryEnterWallRun()
{
	if (!bIsNearWallRunWall
		|| bIsWallClimbing
		|| KaiController->MovementActionTimePressedDown < 0
		|| 180.f - FVectorExt::AngleBetween(WallHipTracer->Hit.ImpactNormal, KaiCharacter->GetActorForwardVector()) <= 30
	)
	{
		return;
	}
	if (IsInMovementState(MOVE_Falling))
	{
		StartWallRunning(false);
	}
	else if (IsInMovementState(MOVE_Walking))
	{
		StartWallRunning(true);
	}
}


void UPLLCharacterMovementComponent::StartWallRunning(const bool FromGround)
{
	if (IsInMovementState(RopeSwinging))
	{
		Rope->Release();
	}
	UKismetSystemLibrary::PrintString(GetWorld(), "Start wallrunn");
	SetMovementState(WallRunning);
	KaiCharacter->bUseControllerRotationYaw      = false;
	auto ChachedVelocity                         = KaiCharacter->GetPLLMovementComp()->Velocity;
	ChachedVelocity.Z                            = 0;
	KaiCharacter->GetPLLMovementComp()->Velocity = ChachedVelocity;

	// SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Z);
	AnimInstance->bCanEnterWallRunLeft       = Sensor->bDidLeftTraceHit;
	AnimInstance->bCanEnterWallRunRight      = Sensor->bDidRightTraceHit;
	AnimInstance->bDidEnterWallRunFromGround = FromGround;
	GravityScale                             = 0.1;
	WallRunLengthCounter                     = 0;
	// StopMovementImmediately();

	if (FromGround)
	{
		AddImpulse(GetWallRunDirection().GetSafeNormal() + FVector(BoostForwardFromGround, 0, BoostUpFromGround), true);
		UKismetSystemLibrary::PrintString(GetWorld(), "BOOSTFROMGROUND");
	}
	OnStartWallRunning.Broadcast();
}


void UPLLCharacterMovementComponent::TickWallRunning(float DeltaTime)
{
	WallRunLengthCounter += DeltaTime;
	// UKismetSystemLibrary::PrintString(GetWorld(),
	//                                   FString::SanitizeFloat(FVectorExt::AngleBetween2D(KaiController->GetControlRotation().Vector(),
	//                                                                                     GetWallRunDirection())));
	GravityScale = FMath::Min(WallRunLengthCounter * 0.1f, 1.f);

	FVector Force = GetWallRunDirection();
	if (Force == FVector::ZeroVector
		|| FVector::DotProduct(Velocity.GetSafeNormal(), Force) < 0
		|| abs(FVectorExt::AngleBetween2D(KaiController->GetControlRotation().Vector(), Force)) > MaxLookAwayAngleForFail
		|| WallRunLengthCounter > WallRunMaxTimeOnWall)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), Force.ToString());
		EndWallRunning(false);
		return;
	}
	Force *= (WallRunSpeed * DeltaTime);
	AddForce(Force);
	// UKismetSystemLibrary::PrintString(GetWorld(), Force.ToString());
}


void UPLLCharacterMovementComponent::EndWallRunning(const bool bWithBoost = false)
{
	if (!IsInMovementState(WallRunning)) return;
	SetMovementState(MOVE_Falling);
	KaiCharacter->bUseControllerRotationYaw = true;
	AnimInstance->bCanEnterWallRunLeft      = false;
	AnimInstance->bCanEnterWallRunRight     = false;
	SetPlaneConstraintNormal(FVector(0));
	GravityScale = 1;

	UKismetSystemLibrary::PrintString(GetWorld(), "End WallRunning");
	if (bWithBoost && WallRunLengthCounter > TimeToGetBoostFromWall)
	{
		const bool bIsLeft     = Sensor->bDidLeftTraceHit;
		FVector    LaunchForce = GetWallRunDirection().RotateAngleAxis(bIsLeft ? BoostAwayFromWallAngle : -BoostAwayFromWallAngle,
		                                                               FVector(0, 0, 1));
		LaunchForce *= FVector(BoostAwayFromWallForce, BoostAwayFromWallForce, 0);
		LaunchForce.Z = BoostUpFromWallForce;

		KaiCharacter->LaunchCharacter(LaunchForce, false, false);
	}
	OnEndWallRunning.Broadcast();
}


void UPLLCharacterMovementComponent::StartSliding()
{
	if (PLLMovementState == GroundSliding || GetWorld()->GetTimerManager().IsTimerActive(SlidingCooldownHandle))
	{
		return;
	}

	SetMovementState(GroundSliding);

	bIsSliding = true;

	KaiCharacter->bUseControllerRotationYaw = false;
	KaiController->bCanWalkRight            = false;
	KaiController->bCanPerformJump          = false;
	GroundFriction                          = SlidingSettings->SlidingGroundFriction;
	BrakingDecelerationWalking              = SlidingSettings->SlidingBrakingDeceleration;

	// Adjust capsule and mesh
	auto Diff = KaiCharacter->GetMesh()->GetComponentLocation().Z;
	KaiCharacter->GetCapsuleComponent()->SetCapsuleHalfHeight(StandingCapsuleHalfHeight / 2);
	KaiCharacter->GetCapsuleComponent()->AddLocalOffset(FVector(0.f, 0.f, -StandingCapsuleHalfHeight / 2.f));
	Diff -= KaiCharacter->GetMesh()->GetComponentLocation().Z;
	KaiCharacter->GetMesh()->AddLocalOffset(FVector(0.f, 0.f, Diff));

	OnStartSliding.Broadcast();
}


void UPLLCharacterMovementComponent::TryEndSliding()
{
	if (!bIsSliding)
	{
		return;
	}

	// Will be checked in Tick()
	bWantsToStand = true;
}


void UPLLCharacterMovementComponent::TryUncrouch()
{
	if (!bWantsToCrouch)
	{
		return;
	}

	// Will be checked in Tick()
	bWantsToStand = true;
}


void UPLLCharacterMovementComponent::EndSliding()
{
	SetMovementState(MOVE_Walking);

	KaiCharacter->bUseControllerRotationYaw = true;
	KaiController->bCanWalkRight            = true;
	KaiController->bCanPerformJump          = true;
	GroundFriction                          = DefaultGroundFriction;
	BrakingDecelerationWalking              = DefaultBrakingDeceleration;

	// Adjust capsule and mesh location
	auto Diff = KaiCharacter->GetMesh()->GetComponentLocation().Z;
	KaiCharacter->GetCapsuleComponent()->SetCapsuleHalfHeight(StandingCapsuleHalfHeight);
	KaiCharacter->GetCapsuleComponent()->AddLocalOffset(FVector(0.f, 0.f, StandingCapsuleHalfHeight / 2.f));
	Diff -= KaiCharacter->GetMesh()->GetComponentLocation().Z;
	KaiCharacter->GetMesh()->AddLocalOffset(FVector(0.f, 0.f, Diff));

	GetWorld()->GetTimerManager().SetTimer(SlidingCooldownHandle, this, &UPLLCharacterMovementComponent::EnableSliding,
	                                       SlidingSettings->SlidingCooldown);

	OnEndSliding.Broadcast();
}


void UPLLCharacterMovementComponent::TickGroundSliding(float DeltaTime)
{
	AddForce(CalculateFloorInfluence(CurrentFloor.HitResult.Normal));

	if (HipTraceResult->bDidHit && FVector::Distance(HipTraceResult->Hit.TraceStart, HipTraceResult->Hit.ImpactPoint) < SlidingSettings->
		DistanceToObjectThreshold)
	{
		TryEndSliding();
		return;
	}

	if (Speed >= SlidingSettings->MaxSlidingSpeed)
	{
		Velocity = Velocity.GetSafeNormal() * SlidingSettings->MaxSlidingSpeed;
	}
	else if (Speed < SlidingSettings->MinSlidingSpeed)
	{
		TryEndSliding();
		return;
	}

	// Floor Tracer sensor does stupid shit
	FHitResult            OutHitResult;
	FVector               Start = KaiCharacter->GetActorLocation();
	FVector               End   = Start + FVector(0.f, 0.f, -5000.f);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(KaiCharacter);

	GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECC_Visibility, QueryParams);

	if (!OutHitResult.bBlockingHit || OutHitResult.Distance > 100.f)
	{
		TryEndSliding();
		return;
	}
}


bool UPLLCharacterMovementComponent::CheckCanSlide()
{
	const bool CanSlide = !IsGroundedIdling() && IsInMovementState(MOVE_Walking) && !IsInMovementState(RopeSwinging) && !
		IsInMovementState(MantleShimmy) && !IsInMovementState(WallRunning) && Speed > 400.f;

	const bool IsMovingForward = FVectorExt::AngleBetween2D(FVector(1.f, 0.f, 0.f), KaiController->AccumulatedDirectionalInput) <=
		SlidingSettings->MaxAllowedStrafingAngle
		&& FMath::Sign(KaiController->AccumulatedDirectionalInput.X) == 1;

	return CanSlide && IsMovingForward;
}


void UPLLCharacterMovementComponent::EnableSliding()
{
	// dummy function, needs no functionality
}


FVector UPLLCharacterMovementComponent::CalculateFloorInfluence(FVector FloorNormal)
{
	const auto UpVector = CharacterOwner->GetActorUpVector();

	if (UpVector.Equals(FloorNormal))
	{
		return FVector::ZeroVector;
	}

	FVector TotalForce = FVector::CrossProduct(FloorNormal, FVector::CrossProduct(FloorNormal, UpVector)).GetSafeNormal();
	TotalForce *= FMath::Clamp((1 - FVector::DotProduct(FloorNormal, UpVector)), 0.f, 1.f) * SlidingSettings->SlidingForce;

	return TotalForce;
}


void UPLLCharacterMovementComponent::ResolveMovementPostSliding()
{
	if (!bWantsToStand)
	{
		return;
	}

	if (bIsSliding)
	{
		bIsSliding = false;

		if (CanStand())
		{
			EndSliding();
			bWantsToStand = false;
		}
		else if (!CanStand())
		{
			EndSliding();
			KaiCharacter->Crouch();
		}
	}
	else if (bWantsToCrouch)
	{
		if (CanStand())
		{
			KaiCharacter->UnCrouch();
			bWantsToStand = false;
		}
	}
}


void UPLLCharacterMovementComponent::StartListeningForSlidingFloor()
{
	bWantsToSlide = true;
}


void UPLLCharacterMovementComponent::StopListeningForSlidingFloor()
{
	bWantsToSlide = false;
}


bool UPLLCharacterMovementComponent::CanStand() const
{
	auto FloorLocation = GetCharacterOwner()->GetActorLocation();
	FloorLocation.Z -= KaiCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	auto HeadLocationStanding = FloorLocation;
	HeadLocationStanding.Z += (StandingCapsuleHalfHeight * 2);
	FHitResult Hit;

	UKismetSystemLibrary::LineTraceSingle(GetWorld(), FloorLocation, HeadLocationStanding, BlockingTraceChannel, false, TArray<AActor*>{},
	                                      EDrawDebugTrace::ForDuration, Hit, true);
	return !Hit.IsValidBlockingHit();
}


bool UPLLCharacterMovementComponent::IsGroundedIdling() const
{
	FVector OutDir;
	float   OutLength;
	KaiController->AccumulatedDirectionalInput.ToDirectionAndLength(OutDir, OutLength);
	return !IsFalling() && FMath::IsNearlyZero(OutLength) && IsInMovementState(MOVE_Walking);
}


void UPLLCharacterMovementComponent::CalculateCurrentSpeed()
{
	FVector OutDir;
	float   OutLength;
	Velocity.ToDirectionAndLength(OutDir, OutLength);

	Speed = OutLength;
}


//Movement State
void UPLLCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (!Rope)
	{
		return;
	}

	// If we walk off a ledge with Rope attached, start swinging
	if (Rope->GetIsAttached() && PreviousMovementMode == MOVE_Walking && MovementMode == MOVE_Falling)
	{
		bEnablePhysicsInteraction = true;
		StartRopeSwinging();
	}

	if (PreviousMovementMode == MOVE_Falling && MovementMode == MOVE_Walking)
	{
		OnStopFalling.Broadcast();
	}

	if (MovementMode == MOVE_Falling && !Rope->GetIsAttached())
	{
		OnStartFalling.Broadcast();
	}
}


void UPLLCharacterMovementComponent::OnMovementStateChanged()
{
	if (IsInMovementState(MOVE_Falling))
	{
	}
}


void UPLLCharacterMovementComponent::SetMovementState(const EMovementMode Mode)
{
	SetMovementMode(Mode);
	PLLMovementState = None;
	OnMovementStateChanged();
}


void UPLLCharacterMovementComponent::SetMovementState(const ECustomMovementMode Mode)
{
	// SetMovementMode(MOVE_Walking);
	PLLMovementState = Mode;
	OnMovementStateChanged();
}


bool UPLLCharacterMovementComponent::IsInMovementState(const EMovementMode Mode) const
{
	return PLLMovementState != None ? false : MovementMode == Mode;
}


bool UPLLCharacterMovementComponent::IsInMovementState(const ECustomMovementMode Mode) const
{
	if (Mode == None && PLLMovementState == None)
	{
		return true;
	}

	return PLLMovementState != None ? PLLMovementState == Mode : false;
}


//From Parkour system

void UPLLCharacterMovementComponent::EnterClimbOver()
{
	if (bIsMantling) return;
	if (IsInMovementState(MOVE_Walking)
		|| IsInMovementState(MOVE_Falling)
		|| IsInMovementState(MantleShimmy))
		return;

	if (bIsHanging)
	{
		OnClimbOverLedge.Broadcast();
		EnterRootMantle();
		AnimInstance->Montage_Play(ClimbOverEdgeFromHangingMontage, 1.0f, EMontagePlayReturnType::MontageLength, 0);
	}
}


void UPLLCharacterMovementComponent::ExitClimbOver()
{
	ExitHanging();
	ExitRootMantle();
	// KaiCharacter->LaunchCharacter(KaiCharacter->GetActorForwardVector().GetSafeNormal() + FVector(50, 0, 0), true, false);
}


void UPLLCharacterMovementComponent::EnterMovementUpAction()
{
	//Inital validity check
	// if (CanEnterMovementAction())
	// {
	// 	return;
	// }
	TryEnterJump();
	TryEnterWallRun();

	//Enter RootParkour
	if (WallFeetTracer->bDidHit)
	{
		const float LedgeHeight        = LedgeTracer->Hit.ImpactPoint.Z;
		const float FloorHeight        = FloorTracer->bDidHit ? FloorTracer->Hit.ImpactPoint.Z : -10000;
		const float LedgeHeightToFloor = LedgeHeight - FloorHeight;

		if (TryEnterWallClimb()) return;
		if (!IsInMovementState(MOVE_Walking))
		{
			return;
		}
		if (WallFeetTracer->bDidHit && LedgeHeightToFloor >= StepOnMin && LedgeHeightToFloor <= StepOnMax)
		{
			bDidJumpComeFromGround = false;
			EnterClimbOn();
		}
	}
}


void UPLLCharacterMovementComponent::EnterMovementDownAction()
{
	if (bIsHanging)
	{
		if (DebugShowParkourInfo)
		{
			UKismetSystemLibrary::PrintString(this, "[MOVEMENT DOWN] EXIT HANGING", true, false, FLinearColor::Red, 5.f);
		}

		ExitHanging();
		return;
	}

	if (bCanStartCrouching)
	{
		if (DebugShowParkourInfo)
		{
			UKismetSystemLibrary::PrintString(this, "[MOVEMENT DOWN] START CROUCH", true, false, FLinearColor::Red, 5.f);
		}

		KaiCharacter->Crouch();
		return;
	}

	if (bCanStartSliding)
	{
		if (DebugShowParkourInfo)
		{
			UKismetSystemLibrary::PrintString(this, "[MOVEMENT DOWN] START SLIDING", true, false, FLinearColor::Red, 5.f);
		}

		StartSliding();
		return;
	}

	if (bWantsToLand)
	{
		StartListeningForSlidingFloor();
	}
}


void UPLLCharacterMovementComponent::TryEnterJump()
{
	if (KaiController->bCanPerformJump && !bIsNearWallRunWall && !bIsHanging && !bIsWallClimbing)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "JUMPMOTHERFUCKER");
		bDidJumpComeFromGround = FloorTracer->bDidHit && !FloorTracer->Hit.Actor->ActorHasTag(JumpUpActorTag);
		bCanPerformLaunch      = false;
		KaiCharacter->Jump();
	}
}


bool UPLLCharacterMovementComponent::CanEnterMovementAction()
{
	return bIsMantling || bIsNearWallRunWall;
}


void UPLLCharacterMovementComponent::TryEnterHanging()
{
	if (!IsInMovementState(MOVE_Falling)) return;


	//Check if Ledge is in Front
	if (!LedgeTracer->bDidHit || bIsHanging) return;
	//Check if check distance from hit to ground
	const float LedgeHeight = LedgeTracer->Hit.ImpactPoint.Z;
	const float FloorHeight = FloorTracer->bDidHit ? FloorTracer->Hit.ImpactPoint.Z : -10000;

	//Check if player is facing the wall
	float AngleBetweenImpactNormalAndForward = UKismetMathLibrary::Vector_CosineAngle2D(KaiCharacter->GetActorForwardVector(),
	                                                                                    -WallHipTracer->Hit.ImpactNormal);
	AngleBetweenImpactNormalAndForward = FMath::RadiansToDegrees(AngleBetweenImpactNormalAndForward);
	// UKismetSystemLibrary::PrintString(GetWorld(), FString::SanitizeFloat(AngleBetweenImpactNormalAndForward));
	if (AngleBetweenImpactNormalAndForward < 30) return;

	//if distance from hit to ground bigger capsule height wait for mantle
	if (LedgeHeight - FloorHeight > CapsuleHh * 2 + 20)
	{
		//Enter Mantle as soon as Pelvis position is on specific height
		const float HeightDif = (Actor->GetActorLocation().Z + PelvisHangingOffset) - LedgeHeight;
		if (abs(HeightDif) > EnterHangingMargin) return;
		if (WallHipTracer->Hit.Distance > Capsule->Bounds.SphereRadius) return;
		//PlayHanging Montage Enter HangingMantle
		EnterHanging();
	}
}


void UPLLCharacterMovementComponent::EnterHanging()
{
	StopMovementImmediately();
	EndWallClimbing();
	EndWallRunning();

	UKismetSystemLibrary::PrintString(GetWorld(), "Enter Hanging");
	SetMovementMode(MOVE_Flying);
	StopMovementImmediately();

	FVector NewActorLoc = WallHipTracer->Hit.ImpactNormal * FVector(DistanceFromWallOffset,
	                                                                DistanceFromWallOffset,
	                                                                0);
	NewActorLoc += FVector(WallHipTracer->Hit.ImpactPoint.X,
	                       WallHipTracer->Hit.ImpactPoint.Y,
	                       LedgeTracer->Hit.ImpactPoint.Z - PelvisHangingOffset);

	FRotator NewActorRot = UKismetMathLibrary::MakeRotFromX(WallHipTracer->Hit.ImpactNormal);
	// FRotator NewActorRot = FRotator::MakeFromEuler(WallHipTracer->Hit.ImpactNormal * -1);
	NewActorRot.Yaw += 180;

	KaiCharacter->bUseControllerRotationYaw = false;

	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision); //testing out this
	KaiController->SetInputReceiveModeToNone();

	KaiController->bCanLookAround      = true;
	KaiController->bCanPerformUpAction = true;

	Actor->SetActorLocation(NewActorLoc);
	Actor->SetActorRotation(NewActorRot);
	AnimInstance->bCanHang = true;
	bIsHanging             = true;
}


void UPLLCharacterMovementComponent::ExitHanging()
{
	if (!bIsHanging) return;

	KaiCharacter->bUseControllerRotationYaw = true;
	SetMovementMode(MOVE_Walking);
	Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	KaiController->SetInputReceiveModeToAll();
	AnimInstance->bCanHang = false;
	bIsHanging             = false;
}


void UPLLCharacterMovementComponent::TryGetOnCube()
{
	// UKismetSystemLibrary::PrintString(GetWorld(), "TryEnterCube");
	if (bIsMantling
		|| bCanPerformLaunch
		|| !bDidJumpComeFromGround
		|| bIsNearWallRunWall
		|| IsInMovementState(WallRunning)
	)
	{
		return;
	}
	//Check if Ledge is in Front

	if (FloorTracer->bDidHit && FloorTracer->Hit.Actor->ActorHasTag(JumpUpActorTag))
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Is above of Interactable");
		bCanPerformLaunch = true;
		return;
	}

	if (bIsHanging || !bIsAccelerating || !LowLedgeTracer->bDidHit) return;

	const float FeetHeight           = Actor->GetActorLocation().Z - CapsuleHh;
	const float LowLedgeHeight       = LowLedgeTracer->Hit.ImpactPoint.Z;
	const float FeetToLowLedgeHeight = LowLedgeHeight - FeetHeight;

	// Check if Character needs to Lerp on to Object 
	if (WallFeetTracer->bDidHit && FeetToLowLedgeHeight > 0 && FeetToLowLedgeHeight < MaxLerpOnObstacleHeight
		&& !bIsLerpingOnObject && bIsInAir
		&& FVector::DotProduct(KaiCharacter->GetActorForwardVector(), KaiCharacter->GetVelocity()) > 0 // Check if Character moves forward
		&& FVector::DotProduct(LowLedgeTracer->Hit.ImpactNormal, FVector(0, 0, 1)) > 0.99
		&& FloorTracer->bDidHit && FeetHeight - FloorTracer->Hit.ImpactPoint.Z > MaxStepHeight)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Lerp on to Object!");
		bIsLerpingOnObject = true;
		LerpOnObjectTarget = LowLedgeTracer->Hit.Location + FVector(0, 0, CapsuleHh);

		EndWallClimbing();
		EnterRootMantle();
		KaiCharacter->DisableInput(KaiController);
	}

	if (!LedgeTracer->bDidHit || !LedgeTracer->Hit.Actor->ActorHasTag(JumpUpActorTag)) return;

	const float LedgeHeight = LedgeTracer->Hit.ImpactPoint.Z;

	if (WallFeetTracer->bDidHit && FeetHeight < LedgeHeight && LedgeHeight - FeetHeight > MaxOffsetForJumpStepOn)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Is n front of Interactable");

		// EnterRootMantle();
		// LerpTarget = LedgeTracer->Hit.Location + FVector(0, 0, CapsuleHh);
		// KaiCharacter->SetInputReceiveModeToNone();
		// bShouldLerpChar = true;
		// OnMantleCharacter.Broadcast(LedgeTracer->Hit.Location + FVector(0, 0, CapsuleHh));
		// KaiCharacter->SetActorLocation(LedgeTracer->Hit.Location + FVector(0, 0, CapsuleHh));
		// FMath::VInterpConstantTo(Actor->GetActorLocation(),
		//                          LedgeTracer->Hit.Location + FVector(0, 0, CapsuleHh),
		//                          FrameTime,
		//                          SpeedToGetPlayerOnObstacle);
		// bCanPerformLaunch = true;
	}
}


void UPLLCharacterMovementComponent::TickMantleOnObject()
{
	if (!bIsLerpingOnObject) return;

	KaiCharacter->SetActorLocation(FMath::VInterpConstantTo(Actor->GetActorLocation(), LerpOnObjectTarget,
	                                                        GetWorld()->GetDeltaSeconds(), SpeedLerpOnObstacle));

	if (FVector::Dist(Actor->GetActorLocation(), LerpOnObjectTarget) < MaxLerpOnObstacleDistance)
	{
		bIsLerpingOnObject = false;
		ExitRootMantle();
		KaiCharacter->EnableInput(KaiController);
		bCanPerformLaunch = true;
	}
}


void UPLLCharacterMovementComponent::ListenToMovementInput()
{
	if (KaiController->MovementActionTimePressedDown > 0)
	{
		TryYeetCharacter();
	}
}


void UPLLCharacterMovementComponent::TryYeetCharacter()
{
	if (bIsInAir || bIsHanging || bIsWallClimbing) return;
	StopMovementImmediately();
	FVector YeetVelocity = KaiCharacter->GetActorForwardVector();
	YeetVelocity *= YeetForceForward;
	YeetVelocity.Z = 1;
	YeetVelocity.Z += YeetForceUp;
	OnJumpUp.Broadcast();
	KaiCharacter->LaunchCharacter(YeetVelocity, true, true);

	bDidJumpComeFromGround = false;
	bCanPerformLaunch      = false;
}


void UPLLCharacterMovementComponent::EnterClimbOn()
{
	EnterRootMantle();
	AnimInstance->Montage_Play(ClimbOnMontage, 1.0f, EMontagePlayReturnType::MontageLength, 0);
}

#pragma region WallClimb

bool UPLLCharacterMovementComponent::TryEnterWallClimb(bool bDidComeOutOfJump)
{
	if (WallEyeTracer->bDidHit
		&& WallHipTracer->bDidHit
		&& WallFeetTracer->bDidHit
		&& (IsInMovementState(MOVE_Walking) || IsInMovementState(MOVE_Falling))
		// && WallEyeTracer->Hit.Actor == WallHipTracer->Hit.Actor
		&& WallHipTracer->Hit.Actor == WallFeetTracer->Hit.Actor
		// && WallHipTracer->Hit.Distance == WallEyeTracer->Hit.Distance
		// && WallEyeTracer->Hit.Distance == WallFeetTracer->Hit.Distance
		&& bIsNearWallRunWall
		// && bDidJumpComeFromGround
		&& KaiController->MovementActionTimePressedDown >= 0
		&& !bIsWallClimbing
	)
	{
		const float AngleToWall = 180.f - FVectorExt::AngleBetween(WallHipTracer->Hit.ImpactNormal, KaiCharacter->GetActorForwardVector());
		UKismetSystemLibrary::PrintString(this, FString::SanitizeFloat(AngleToWall), true, false,
		                                  FLinearColor::Yellow, 5.f);

		if (FMath::Abs(AngleToWall) <= 20.f)
		{
			bDidJumpComeFromGround = false;
			EnterWallClimbing(bDidComeOutOfJump);
			return true;
		}
	}
	return false;
}


void UPLLCharacterMovementComponent::EnterWallClimbing(bool bWithReducedTime)
{
	if (bIsWallClimbing || bIsMantling || bIsHanging) return; //|| !bCanPerformWallClimb
	bIsWallClimbing                  = true;
	AnimInstance->bCanEnterWallClimb = true;
	bCanPerformWallClimb             = false;

	KaiController->bCanWalkForward          = false;
	KaiController->bCanWalkRight            = false;
	KaiCharacter->bUseControllerRotationYaw = false;

	UKismetSystemLibrary::PrintString(GetWorld(), "EnterWallClimb");

	// SetPlaneConstraintNormal(FVector(1, 1, 0));
	OnStartWallClimbing.Broadcast(bWithReducedTime);
}


void UPLLCharacterMovementComponent::TickWallClimbing()
{
}


void UPLLCharacterMovementComponent::EndWallClimbing()
{
	if (!bIsWallClimbing) return;
	UKismetSystemLibrary::PrintString(GetWorld(), "End WallClimbing");
	KaiController->bCanWalkForward          = true;
	KaiController->bCanWalkRight            = true;
	KaiCharacter->bUseControllerRotationYaw = true;

	OnEndWallClimbing.Broadcast();
	AnimInstance->bCanEnterWallClimb = false;
	bIsWallClimbing                  = false;
	SetMovementState(MOVE_Falling);
	SetPlaneConstraintNormal(FVector(0));
}

#pragma endregion

void UPLLCharacterMovementComponent::EnterRootMantle()
{
	// UKismetSystemLibrary::PrintString(GetWorld(), "ENTER ROOT MANTLE");
	bIsMantling                       = true;
	AnimInstance->HeadAimOffsetWeight = 0;
	StopMovementImmediately();
	SetMovementMode(MOVE_Flying);
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void UPLLCharacterMovementComponent::ExitRootMantle()
{
	// UKismetSystemLibrary::PrintString(GetWorld(), "EXIT ROOT MANTLE");
	bIsMantling                       = false;
	AnimInstance->HeadAimOffsetWeight = 1;
	Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetMovementMode(MOVE_Walking);
	AnimInstance->StopAllMontages(0);
}


void UPLLCharacterMovementComponent::QueueLandingRequest()
{
	LastPerformedLandingScore = CurrentLandingScore;

	if (GetWorld()->GetTimerManager().IsTimerActive(LandingCooldownTimerHandle))
	{
		LastPerformedLandingScore = 0.f;
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(LandingCooldownTimerHandle, this, &UPLLCharacterMovementComponent::EnableLandingAttempts,
	                                       LandingSettings->LandingCooldown);
}


void UPLLCharacterMovementComponent::EnableLandingAttempts()
{
	// Dummy Function / no functionality needed
	// Timer used in "QueueLandingRequest" will be set inactive automatically when this is called
}


float UPLLCharacterMovementComponent::GetCurrentLandingScore() const
{
	return CurrentLandingScore;
}


float UPLLCharacterMovementComponent::GetLastPerformedLandingScore() const
{
	return LastPerformedLandingScore;
}


void UPLLCharacterMovementComponent::CalculateCurrentLandingScore()
{
	if (!FloorTracer->Hit.IsValidBlockingHit())
	{
		return;
	}

	const float CurrentDistanceToGround = FloorTracer->Hit.Distance;
	if (!FMath::IsNearlyEqual(CurrentDistanceToGround, LandingSettings->PerfectLandingGroundDistance,
	                          LandingSettings->PerfectLandingThreshold))
	{
		CurrentLandingScore = 0.f;
		return;
	}

	float InRangeStart;
	float InRangeEnd;

	if (CurrentDistanceToGround >= LandingSettings->PerfectLandingGroundDistance)
	{
		InRangeStart = LandingSettings->PerfectLandingGroundDistance + LandingSettings->PerfectLandingThreshold;
		InRangeEnd   = LandingSettings->PerfectLandingGroundDistance;
	}
	else
	{
		InRangeStart = LandingSettings->PerfectLandingGroundDistance - LandingSettings->PerfectLandingThreshold;
		InRangeEnd   = LandingSettings->PerfectLandingGroundDistance;
	}

	CurrentLandingScore = FMath::GetMappedRangeValueClamped(TRange<float>(InRangeStart, InRangeEnd), TRange<float>(0.f, 1.f),
	                                                        FloorTracer->Hit.Distance);
}


bool UPLLCharacterMovementComponent::CheckLandingRequirements(float DeltaTime)
{
	bool bRequirementsMet = true;

	FallingTime += DeltaTime;

	const float DeltaHeight = KaiCharacter->GetActorLocation().Z - LastPosition.Z;
	if (DeltaHeight < 0.f)
	{
		DownFallingDistance += FMath::Abs(DeltaHeight);
	}

	if (DownFallingDistance >= LandingSettings->FallingHeightDeathThreshold)
	{
		bWillDieOnImpact = true;
	}
	else
	{
		bWillDieOnImpact = false;
	}

	if (DebugShowParkourInfo)
	{
		UKismetSystemLibrary::PrintString(this, FString("FALLING TIME: ").Append(FString::SanitizeFloat(FallingTime)), true, false,
		                                  FLinearColor::Red, 0.f);
		UKismetSystemLibrary::PrintString(this, FString("DOWN FALLING DIST: ").Append(FString::SanitizeFloat(DownFallingDistance)), true,
		                                  false,
		                                  FLinearColor::Red, 0.f);
	}

	if (LandingSettings->bUseTimeRequirement)
	{
		bRequirementsMet = bRequirementsMet && (FallingTime >= LandingSettings->TimeRequirement);
	}
	if (LandingSettings->bUseHeightRequirement)
	{
		bRequirementsMet = bRequirementsMet && (DownFallingDistance >= LandingSettings->HeightRequirement);
	}

	return bRequirementsMet;
}


void UPLLCharacterMovementComponent::PerformLanding()
{
	StopListeningForLanding();
	StopListeningForSlidingFloor();

	// bCanPerformWallClimb = true;
	// 
	// Cache needed values
	const bool  bWasPerfect                  = LastPerformedLandingScore > 0.f ? true : false;
	const float PerfectLandingScoreCache     = LastPerformedLandingScore;
	const bool  bMetLandingRequirementsCache = bMetLandingRequirements;

	if (!bMetLandingRequirementsCache || bIsSliding == true || bWillDieOnImpact)
	{
		return;
	}

	OnPerformLanding.Broadcast(bWasPerfect, PerfectLandingScoreCache);

	KaiController->SetInputReceiveModeToNone();

	if (DebugShowParkourInfo)
	{
		UKismetSystemLibrary::PrintString(this, bWasPerfect ? "PERFECT LANDING" : "FAILED LANDING", true, false, FLinearColor::Red);
	}
}


void UPLLCharacterMovementComponent::OnStartFallingFunction(FVector PlayerLocation)
{
	JumpStartLocation = KaiCharacter->GetActorLocation();

	StartListeningForLanding();
}


void UPLLCharacterMovementComponent::HandlePerfectLanding()
{
	KaiController->SetInputReceiveModeToAll();
	MaxWalkSpeed = GetMaxFlowRunSpeed();
}


void UPLLCharacterMovementComponent::HandleHardLanding()
{
	KaiController->SetInputReceiveModeToAll();
}


float UPLLCharacterMovementComponent::GetCurrentFallingHeight() const
{
	if (IsInMovementState(RopeSwinging))
	{
		return 0.f;
	}

	return DownFallingDistance;
}


void UPLLCharacterMovementComponent::ResetLandingValues()
{
	LastPerformedLandingScore = 0.f;
	FallingTime               = 0.f;
	DownFallingDistance       = 0.f;
	bMetLandingRequirements   = false;
}


void UPLLCharacterMovementComponent::StartListeningForLanding()
{
	bWantsToLand      = true;
	JumpStartLocation = KaiCharacter->GetActorLocation();

	ResetLandingValues();
}


void UPLLCharacterMovementComponent::StopListeningForLanding()
{
	bWantsToLand = false;
}


void UPLLCharacterMovementComponent::HandleOnCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                                        UPrimitiveComponent* OtherComp, FVector    NormalImpulse, const FHitResult& Hit)
{
	if (bWillDieOnImpact)
	{
		bWillDieOnImpact = false;

		OnDiedOnImpact.Broadcast();
	}
}
