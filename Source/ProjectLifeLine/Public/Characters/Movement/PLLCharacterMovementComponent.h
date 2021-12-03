// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerControllers/PLLPlayerController.h"
#include "UDataAsset/LandingSettings.h"
#include "UDataAsset/PLLHitResult.h"
#include "UDataAsset/SlidingSettings.h"

#include "PLLCharacterMovementComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBoostFromPerfectReleaseAngle);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartFalling);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopFalling);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartRopeSwinging);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndRopeSwinging);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartWallRunning);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndWallRunning);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartWallClimbing, bool, bWithReducedTime);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndWallClimbing);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartSliding);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndSliding);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMantleCharacter, FVector, MantleTargetPosition);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJumpUp);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClimbOverLedge);

// LandingScore = 0 if bWasPerfect == false
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPerformLanding, bool, bWasPerfect, float, LandingScore);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDiedOnImpact);


UENUM(BlueprintType)
enum ECustomMovementMode
{
	RopeClimbing UMETA(DisplayName = "RopeClimbing"),
	RopeSwinging UMETA(DisplayName = "RopeSwinging"),
	MantleShimmy UMETA(DisplayName = "MantleShimmy"),
	WallRunning UMETA(DisplayName = "WallRunning"),
	GroundSliding UMETA(DisplayName = "GroundSliding"),
	None,
};

class UPLLCharacterSensorComponent;
class UPLLAnimInstance;
class APLLRope;
class APLLCharacter;
class APLLCameraManager;

UCLASS()
class PROJECTLIFELINE_API UPLLCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
protected:
#pragma region WallRunning Properties

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRunning")
	float WallRunSpeed = 3000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRunning")
	float BoostUpFromGround = 150;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRunning")
	float BoostForwardFromGround = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRunning")
	float BoostAwayFromWallForce = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRunning")
	float BoostUpFromWallForce = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRunning")
	float BoostAwayFromWallAngle = 70;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRunning")
	float TimeToGetBoostFromWall = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRunning")
	float MaxEnterAngleToWall = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRunning")
	float MaxLookAwayAngleForFail = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRunning")
	float WallRunMaxTimeOnWall = 50;

#pragma endregion

#pragma region Character Movement Properties

	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxFlowRunSpeed;
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float TimeToAccelerateToMaxFlow;

#pragma endregion

#pragma region General and Sensors Properties

	UPROPERTY(EditAnywhere, Category = "General Settings", meta = (DisplayName = "Pelvis Socket Name"))
	FName Pelvis;

	UPROPERTY(EditAnywhere, Category = "General Settings|Sensors")
	UPLLHitResult* WallFeetTracer;

	UPROPERTY(EditAnywhere, Category = "General Settings|Sensors")
	UPLLHitResult* WallHipTracer;

	UPROPERTY(EditAnywhere, Category = "General Settings|Sensors")
	UPLLHitResult* WallEyeTracer;

	UPROPERTY(EditAnywhere, Category = "General Settings|Sensors")
	UPLLHitResult* FloorTracer;

	UPROPERTY(EditAnywhere, Category = "General Settings|Sensors")
	UPLLHitResult* HeadTracer;

	UPROPERTY(EditAnywhere, Category = "General Settings|Sensors")
	UPLLHitResult* LedgeTracer;

	UPROPERTY(EditAnywhere, Category = "General Settings|Sensors")
	UPLLHitResult* LowLedgeTracer;

#pragma endregion

#pragma region Mantling Properties
	UPROPERTY(EditAnywhere, Category = "Hanging/Climb On")
	float PelvisHangingOffset = 130;

	//How close the player must be to the hanging position to actually enter the hang
	UPROPERTY(EditAnywhere, Category = "Hanging/Climb Over")
	float EnterHangingMargin = 10;

	UPROPERTY(EditAnywhere, Category = "Hanging/Climb Over")
	float DistanceFromWallOffset = 28;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hanging/Climb Over")
	UAnimMontage* ClimbOverEdgeFromHangingMontage;

#pragma endregion

#pragma region Parkour Properties

	UPROPERTY(EditAnywhere, Category = "Parkour|Jump On/Yeet")
	float StepOnMin = 70;

	UPROPERTY(EditAnywhere, Category = "Parkour|Jump On/Yeet")
	float StepOnMax = 100;

	UPROPERTY(EditAnywhere, Category = "Parkour|Jump On/Yeet")
	FName JumpUpActorTag;

	UPROPERTY(EditAnywhere, Category = "Parkour|Jump On/Yeet")
	float MaxOffsetForJumpStepOn;

	UPROPERTY(EditAnywhere, Category = "Parkour|Jump On/Yeet")
	float SpeedToGetPlayerOnObstacle;

	UPROPERTY(EditAnywhere, Category = "Parkour|Jump On/Yeet")
	float SpeedLerpOnObstacle = 270;

	UPROPERTY(EditAnywhere, Category = "Parkour|Jump On/Yeet")
	float MaxLerpOnObstacleDistance = 10;

	UPROPERTY(EditAnywhere, Category = "Parkour|Jump On/Yeet")
	float MaxLerpOnObstacleHeight = 65;

	UPROPERTY(EditAnywhere, Category = "Parkour|Jump On/Yeet")
	float YeetForceForward;

	UPROPERTY(EditAnywhere, Category = "Parkour|Jump On/Yeet")
	float YeetForceUp;

#pragma endregion

#pragma region RootUp Properties
	//RootUp
	UPROPERTY(EditAnywhere, Category = "Parkour|Root Up")
	float MaxDistanceFromWallToEnter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parkour|Root Up")
	float WallClimbLengthSeconds = 1.2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parkour|Root Up")
	float WallClimbForce = 200;

	UPROPERTY(EditAnywhere, Category = "Parkour|Root Up")
	UAnimMontage* ClimbOnMontage;

#pragma endregion

#pragma region Landing Properties

	UPROPERTY(EditAnywhere, Category = "Landing")
	ULandingSettings* LandingSettings;

#pragma endregion

	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


#pragma region Event Dispatchers

	UPROPERTY(BlueprintAssignable, Category="Event Dispatchers")
	FOnStartFalling OnStartFalling;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FOnStopFalling OnStopFalling;

	UPROPERTY(BlueprintAssignable, Category="Event Dispatchers")
	FOnStartRopeSwinging OnStartRopeSwinging;

	UPROPERTY(BlueprintAssignable, Category="Event Dispatchers")
	FOnEndRopeSwinging OnEndRopeSwinging;

	UPROPERTY(BlueprintAssignable, Category="Event Dispatchers")
	FOnStartWallRunning OnStartWallRunning;

	UPROPERTY(BlueprintAssignable, Category="Event Dispatchers")
	FOnEndWallRunning OnEndWallRunning;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FOnStartSliding OnStartSliding;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FOnEndSliding OnEndSliding;


#pragma endregion


#pragma region Running
	UFUNCTION()
	void ExitFlowState();

	float GetMaxFlowRunSpeed() const { return MaxFlowRunSpeed; }

#pragma endregion

#pragma region Rope Swinging
	UFUNCTION()
	void StartRopeSwinging();

	UFUNCTION()
	void EndRopeSwinging(float ReleaseAngle);

#pragma endregion

#pragma region WallRun

	UFUNCTION(BlueprintCallable)
	FVector GetWallRunDirection();

	UFUNCTION(BlueprintCallable)
	void TryEnterWallRun();

	UFUNCTION(BlueprintCallable)
	void StartWallRunning(bool FromGround);

	UFUNCTION(BlueprintCallable)
	void EndWallRunning(bool bWithBoost);

#pragma endregion

#pragma region Sliding and Crouch

	UFUNCTION(BlueprintCallable)
	void StartSliding();

	UFUNCTION(BlueprintCallable)
	void TryEndSliding();

	UFUNCTION(BlueprintCallable)
	void TryUncrouch();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (CompactNodeTitle = "CanStand"))
	bool CanStand() const;

	void ResolveMovementPostSliding();

	UFUNCTION(BlueprintCallable)
	void StartListeningForSlidingFloor();

	UFUNCTION(BlueprintCallable)
	void StopListeningForSlidingFloor();

	UFUNCTION(BlueprintCallable)
	bool IsGroundedIdling() const;

#pragma endregion

#pragma region Rope

	UFUNCTION()
	void  SetRope(APLLRope* NewRope);
	float GetCurrentSpeed() const { return Speed; }

	void ApplyGrappleBoost(FVector BoostDirection);

#pragma endregion

#pragma region State
	void SetMovementState(EMovementMode Mode);
	void SetMovementState(ECustomMovementMode Mode);
	bool IsInMovementState(EMovementMode Mode) const;
	bool IsInMovementState(ECustomMovementMode Mode) const;

#pragma endregion

	//Properties
	UPROPERTY(BlueprintReadWrite)
	bool bIsNearWallRunWall;

	UPROPERTY(BlueprintReadOnly)
	FVector Direction;

	UPROPERTY(BlueprintReadOnly)
	float Speed;

	UPROPERTY(BlueprintReadOnly)
	bool bCanStartCrouching;

	UPROPERTY(BlueprintReadOnly)
	bool bCanStartSliding;

	UPROPERTY(BlueprintReadOnly)
	bool bIsSliding;

	UPROPERTY(BlueprintReadOnly)
	bool bWantsToSlide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sliding")
	TEnumAsByte<ETraceTypeQuery> BlockingTraceChannel;

	UPROPERTY(EditDefaultsOnly)
	UPLLHitResult* HipTraceResult;

	UPROPERTY(EditAnywhere, Category = "Sliding")
	USlidingSettings* SlidingSettings;

	//From Parkour System
	UFUNCTION(BlueprintCallable)
	void EnterClimbOver();

	UFUNCTION(BlueprintCallable)
	void ExitClimbOver();

	UFUNCTION(BlueprintCallable)
	bool TryEnterWallClimb(bool bDidComeOutOfJump = false);

	UFUNCTION(BlueprintCallable)
	void EnterMovementUpAction();

	UFUNCTION(BlueprintCallable)
	void EnterMovementDownAction();

	UFUNCTION(BlueprintCallable)
	void TryEnterJump();

	// Entry point when trying to a perfect landing
	UFUNCTION(BlueprintCallable)
	void QueueLandingRequest();

	// Returns between 1 (Perfect) and 0 (failed)
	UFUNCTION(CallInEditor, BlueprintPure, meta = ( CompactNodeTitle = "LandingScore"))
	float GetCurrentLandingScore() const;

	// Returns between 1 (Perfect) and 0 (failed)
	UFUNCTION(CallInEditor, BlueprintPure, meta = (CompactNodeTitle = "LastPerformedLandingScore"))
	float GetLastPerformedLandingScore() const;

	UFUNCTION(BlueprintCallable)
	void HandlePerfectLanding();

	UFUNCTION(BlueprintCallable)
	void HandleHardLanding();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentFallingHeight() const;

	void ResetLandingValues();

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnMantleCharacter OnMantleCharacter;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnJumpUp OnJumpUp;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FOnStartWallClimbing OnStartWallClimbing;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FOnEndWallClimbing OnEndWallClimbing;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FOnPerformLanding OnPerformLanding;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FOnClimbOverLedge OnClimbOverLedge;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FOnDiedOnImpact OnDiedOnImpact;

	UPROPERTY(BlueprintReadOnly, Category = "Landing")
	bool bMetLandingRequirements;

	UPROPERTY(BlueprintReadOnly, Category = "Landing")
	float LastPerformedLandingScore;

private:
	UPROPERTY()
	APLLRope* Rope;

	UPROPERTY()
	APLLCharacter* KaiCharacter;

	UPROPERTY()
	APLLCameraManager* CameraManager;

	UPROPERTY()
	APLLPlayerController* KaiController;

	UPROPERTY()
	UPLLCharacterSensorComponent* Sensor;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UPLLAnimInstance* AnimInstance;


	ECustomMovementMode PLLMovementState;

	//WallRUn
	float WallRunLengthCounter;
	bool  bCanWallRunAgain = true;

	//FlowRunning
	float DefaultMaxWalkSpeed;

	bool bIsInFlowState;

	FRotator RotationDelta;
	FRotator LastRotation;

	// Sliding
	float StandingCapsuleHalfHeight;
	float DefaultGroundFriction;
	float DefaultBrakingDeceleration;
	bool  bWantsToStand;

	FTimerHandle SlidingCooldownHandle;


	void TickRopeSwinging(float DeltaTime);
	void TickWallRunning(float DeltaTime);
	void TickRunning(float DeltaTime);
	void TickGroundSliding(float DeltaTime);
	void EndSliding();
	bool CheckCanSlide();

	UFUNCTION()
	void EnableSliding();

	FVector CalculateFloorInfluence(FVector FloorNormal);

	void CalculateCurrentSpeed();

	void ApplyPerfectRopeReleaseBoost();

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	void         OnMovementStateChanged();

	//From Parkour system
	float FrameTime;
	bool  bIsInAir;
	bool  bIsAccelerating;
	bool  bIsMantling;
	bool  bIsLerpingOnObject;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	bool bIsHanging;

	bool    bCanStepUp;
	bool    bCanPerformLaunch;
	bool    bCanPerformWallClimb;
	float   CurrentLandingScore;
	float   FallingTime;
	float   DownFallingDistance;
	FVector LastPosition;
	FVector JumpStartLocation;
	bool    bWantsToLand;
	bool    bWillDieOnImpact;


	UPROPERTY()
	AActor* Actor;

	UPROPERTY()
	UCapsuleComponent* Capsule;

	UPROPERTY()
	USkeletalMeshComponent* Mesh;

	float CapsuleHh;

	//Functions
	bool CanEnterMovementAction();
	void TryEnterHanging();
	void EnterHanging();

	UFUNCTION(BlueprintCallable)
	void ExitHanging();

	//Climb Up
	bool bIsWallClimbing;
	void EnterWallClimbing(bool bWithReducedTime = false);
	void TickWallClimbing();

	UFUNCTION(BlueprintCallable)
	void EndWallClimbing();

	//NormalParkour
	FVector LerpOnObjectTarget;
	bool    bDidJumpComeFromGround;
	void    TryGetOnCube();
	void    TickMantleOnObject();
	void    ListenToMovementInput();
	void    TryYeetCharacter();

	//RootParkour
	void EnterClimbOn();

	void EnterRootMantle();

	UFUNCTION(BlueprintCallable)
	void ExitRootMantle();

	// Landing

	UFUNCTION()
	void EnableLandingAttempts();

	void CalculateCurrentLandingScore();

	bool CheckLandingRequirements(float DeltaTime);

	UFUNCTION()
	void PerformLanding();

	UFUNCTION()
	void OnStartFallingFunction(FVector PlayerLocation);

	UFUNCTION()
	void StartListeningForLanding();

	UFUNCTION()
	void StopListeningForLanding();

	UFUNCTION()
	void HandleOnCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	                        const FHitResult&    Hit);

	UPROPERTY()
	FTimerHandle LandingCooldownTimerHandle;
};
