// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllers/PLLPlayerController.h"
#include "PLLCameraManager.h"
// #include "ActorComponent/PLLParkourSystemComponent.h"
#include "Characters/PLLAnimInstance.h"
#include "Characters/PLLBulletTimeComponent.h"
#include "Characters/PLLCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Characters/Movement/PLLCharacterMovementComponent.h"
#include "Gamemodes/PLLGameModePlay.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SceneComponent/PLLRopeShooterComponent.h"
#include "Components/CapsuleComponent.h"


APLLPlayerController::APLLPlayerController()
{
	PlayerCameraManagerClass = APLLCameraManager::StaticClass();
}


void APLLPlayerController::BeginPlay()
{
	KaiCharacter = Cast<APLLCharacter>(GetPawn());
	check(KaiCharacter);

	KaiMovComponent = KaiCharacter->GetPLLMovementComp();
	check(KaiMovComponent);

	CameraManager = GetPllCameraManager();
	check(CameraManager)

	// ParkourSystemComp = KaiCharacter->GetPLLParkourSystemComp();
	// check(ParkourSystemComp);

	KaiMovComponent->OnDiedOnImpact.AddDynamic(this, &APLLPlayerController::HandleOnDeath);

	SetupInput();

	DefaultMeshRelativeLocation = KaiCharacter->GetMesh()->GetRelativeLocation();
	DefaultMeshRelativeRotation = KaiCharacter->GetMesh()->GetRelativeRotation();
}


void APLLPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!AccumulatedDirectionalInput.IsNearlyZero())
	{
		LastDirectionalInput = AccumulatedDirectionalInput;
	}
}


void APLLPlayerController::SetupInput()
{
	InputComponent->BindAction("CameraViewAction", IE_Released, this, &APLLPlayerController::SwitchCameraView);

	const auto BulletTimeComp = KaiCharacter->FindComponentByClass<UPLLBulletTimeComponent>();
	check(BulletTimeComp);
	InputComponent->BindAction("BulletTimeAction", IE_Pressed, BulletTimeComp, &UPLLBulletTimeComponent::StartBulletTime);

	// const auto RopeShooterComp = KaiCharacter->FindComponentByClass<UPLLRopeShooterComponent>();
	// check(RopeShooterComp)
	// InputComponent->BindAction("GrappleBoostAction", IE_Pressed, RopeShooterComp, &UPLLRopeShooterComponent::TryApplyGrappleBoost);
}


//Camera
void APLLPlayerController::SwitchCameraView()
{
	check(CameraManager);

	CameraManager->SwitchCameraView();
}


void APLLPlayerController::SwitchToCamera(CameraViewMode NewCameraViewMode)
{
	if (NewCameraViewMode == FirstPerson)
	{
		CameraManager->ApplyFirstPersonView();
	}
	else if (NewCameraViewMode == ThirdPerson)
	{
		CameraManager->ApplyThirdPersonView();
	}
}


void APLLPlayerController::SetInputReceiveModeToAll()
{
	bCanWalkRight         = true;
	bCanWalkForward       = true;
	bCanPerformJump       = true;
	bCanPerformUpAction   = true;
	bCanPerformDownAction = true;
	bCanLookAround        = true;
	bCanFireRope          = true;
}


void APLLPlayerController::SetInputReceiveModeToNone()
{
	bCanWalkRight         = false;
	bCanWalkForward       = false;
	bCanPerformJump       = false;
	bCanPerformUpAction   = false;
	bCanPerformDownAction = false;
	bCanLookAround        = false;
	bCanFireRope          = false;
}


void APLLPlayerController::DisableInputMode()
{
	bCacheCanWalkRight         = bCanWalkRight;
	bCacheCanWalkForward       = bCanWalkForward;
	bCacheCanPerformJump       = bCanPerformJump;
	bCacheCanPerformUpAction   = bCanPerformUpAction;
	bCacheCanPerformDownAction = bCanPerformDownAction;
	bCacheCanLookAround        = bCanLookAround;
	bCacheCanFireRope          = bCanFireRope;
	SetInputReceiveModeToNone();
}


void APLLPlayerController::EnableInputMode()
{
	bCanWalkRight         = bCacheCanWalkRight;
	bCanWalkForward       = bCacheCanWalkForward;
	bCanPerformJump       = bCacheCanPerformJump;
	bCanPerformUpAction   = bCacheCanPerformUpAction;
	bCanPerformDownAction = bCacheCanPerformDownAction;
	bCanLookAround        = bCacheCanLookAround;
	bCanFireRope          = bCacheCanFireRope;
}


void APLLPlayerController::AddForwardInput(const float Value)
{
	AccumulatedDirectionalInput.X = Value;
	PropagateInputVector();
}


void APLLPlayerController::AddRightInput(const float Value)
{
	AccumulatedDirectionalInput.Y = Value;
	PropagateInputVector();
}


void APLLPlayerController::PropagateInputVector()
{
	const FRotator WorldControlRotation(0, GetControlRotation().Yaw, 0);
	const FVector  WorldForward = UKismetMathLibrary::GetForwardVector(WorldControlRotation);
	const FVector  WorldRight   = UKismetMathLibrary::GetRightVector(WorldControlRotation);

	KaiMovComponent->AddInputVector((WorldForward * AccumulatedDirectionalInput.X) + (WorldRight * AccumulatedDirectionalInput.Y));
}


void APLLPlayerController::HandleOnDeath()
{
	KaiCharacter->GetMesh()->SetCollisionProfileName("Ragdoll");
	KaiCharacter->GetMesh()->SetSimulatePhysics(true);
	SetInputReceiveModeToNone();
	SwitchToCamera(ThirdPerson);

	auto GameModePlay = Cast<APLLGameModePlay>(UGameplayStatics::GetGameMode(this));
	check(GameModePlay);

	GetWorld()->GetTimerManager().SetTimer(TimeDeadBeforeRespawnHandle, GameModePlay, &APLLGameModePlay::ResetToLastCheckpoint,
	                                       TimeDeadBeforeRespawn);
}


void APLLPlayerController::OnRespawn()
{
	KaiMovComponent->ResetLandingValues();

	KaiCharacter->GetMesh()->SetSimulatePhysics(false);
	KaiCharacter->GetMesh()->SetCollisionProfileName("CharacterMesh");
	KaiCharacter->GetMesh()->AttachToComponent(KaiCharacter->GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	KaiCharacter->GetMesh()->SetRelativeLocationAndRotation(DefaultMeshRelativeLocation, DefaultMeshRelativeRotation);

	SetInputReceiveModeToAll();
	SwitchToCamera(FirstPerson);

	KaiCharacter->GetPLLMovementComp()->SetMovementState(None);
	KaiCharacter->GetPLLMovementComp()->SetMovementMode(MOVE_Walking);
	KaiCharacter->GetPLLAnimInstance()->ResetDynamics(ETeleportType::ResetPhysics);
}
