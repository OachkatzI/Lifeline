// Copyright Epic Games, Inc. All Rights Reserved.


#include "Characters/PLLCharacter.h"
#include "PlayerControllers/PLLPlayerController.h"
#include "Characters/Movement/PLLCharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/PLLAnimInstance.h"
#include "Characters/PLLBulletTimeComponent.h"
#include "SceneComponent/PLLRopeShooterComponent.h"


static bool DebugShowCharacterInfo = true;
FAutoConsoleVariableRef CVARDebugCharacterInfoPrinting(TEXT("PLL.CharacterInfo"), DebugShowCharacterInfo, TEXT("Print character info to screen"), ECVF_Cheat);


APLLCharacter::APLLCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPLLCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationYaw   = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll  = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate              = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity             = 600.f;
	GetCharacterMovement()->AirControl                = 0.2f;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh(), "CameraSocket");
	FirstPersonCamera->bUsePawnControlRotation = true;

	ThirdPersonCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("ThirdPersonCameraBoom"));
	ThirdPersonCameraBoom->SetupAttachment(RootComponent);
	ThirdPersonCameraBoom->TargetArmLength         = 300.0f;
	ThirdPersonCameraBoom->bUsePawnControlRotation = true;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(ThirdPersonCameraBoom, USpringArmComponent::SocketName);
	ThirdPersonCamera->bUsePawnControlRotation = false;

	ThirdPersonCamera->SetActive(false);
	FirstPersonCamera->SetActive(true);

	BulletTimeComponent = CreateDefaultSubobject<UPLLBulletTimeComponent>(TEXT("BulletTimeComponent"));

	RopeShooterComponent = CreateDefaultSubobject<UPLLRopeShooterComponent>(TEXT("RopeShooterComponent"));
	RopeShooterComponent->SetupAttachment(GetMesh(), "RopeSocket");
}


void APLLCharacter::BeginPlay()
{
	Super::BeginPlay();

	KaiMovementComp = GetPLLMovementComp();
	check(KaiMovementComp);

	KaiController = GetPLLController();
	check(KaiController);
}


void APLLCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (DebugShowCharacterInfo)
	{
		UKismetSystemLibrary::PrintString(this, FString("[CharacterWorldRotation] ").Append(GetActorRotation().ToString()), true, false, FLinearColor::Green, 0.f);
		UKismetSystemLibrary::PrintString(this, FString("[ControlWorldRotation] ").Append(GetControlRotation().ToString()), true, false, FLinearColor::Green, 0.f);
	}

	PreUpdateCamera(DeltaSeconds); 
}


void APLLCharacter::AddControllerYawInput(float Val)
{
	if (KaiMovementComp->IsInMovementState(GroundSliding))
	{
		if (Val > 0.f)
		{
			if (CameraLocalRotation.Yaw >= 80.f)
			{
				Val = 0.f;
			}
		}
		else if (Val < 0.f)
		{
			if (CameraLocalRotation.Yaw <= -80.f)
			{
				Val = 0.f;
			}
		}
	}

	Super::AddControllerYawInput(Val);
}


void APLLCharacter::PreUpdateCamera(float DeltaTime)
{
	if (!KaiController)
	{
		return;
	}

	//-------------------------------------------------------
	// Compute rotation for Mesh AIM Offset
	//-------------------------------------------------------
	FRotator ControllerRotation = KaiController->GetControlRotation();
	FRotator NewRotation = ControllerRotation;

	// Get current controller rotation and process it to match the Character
	NewRotation.Yaw   = CameraProcessYaw(ControllerRotation.Yaw);
	NewRotation.Pitch = CameraProcessPitch(ControllerRotation.Pitch);
	NewRotation.Normalize();

	// Clamp new rotation
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch, -90.0f + CameraThreshold, 90.0f - CameraThreshold);
	NewRotation.Yaw   = FMath::Clamp(NewRotation.Yaw, -91.0f, 91.0f);

	//Update local variable, will be retrieved by AnimBlueprint
	CameraLocalRotation = NewRotation;
}


float APLLCharacter::CameraProcessPitch(float Input)
{
	// Recenter value
	if (Input > 269.99f)
	{
		Input -= 270.0f;
		Input = 90.0f - Input;
		Input *= -1.0f;
	}

	return Input;
}


float APLLCharacter::CameraProcessYaw(float Input)
{
	//Get direction vector from Controller and Character
	FVector Direction1 = GetActorRotation().Vector();
	FVector Direction2 = FRotator(0.0f, Input, 0.0f).Vector();

	//Compute the Angle difference between the two dirrection
	float Angle = FMath::Acos(FVector::DotProduct(Direction1, Direction2));
	Angle       = FMath::RadiansToDegrees(Angle);

	//Find on which side is the angle difference (left or right)
	FRotator Temp       = GetActorRotation() - FRotator(0.0f, 90.0f, 0.0f);
	FVector  Direction3 = Temp.Vector();

	float Dot = FVector::DotProduct(Direction3, Direction2);

	//Invert angle to switch side
	if (Dot > 0.0f)
	{
		Angle *= -1;
	}

	return Angle;
}


APLLPlayerController* APLLCharacter::GetPLLController() const
{
	return Cast<APLLPlayerController>(GetController());
}


UPLLCharacterMovementComponent* APLLCharacter::GetPLLMovementComp() const
{
	return Cast<UPLLCharacterMovementComponent>(GetMovementComponent());
}


UPLLAnimInstance* APLLCharacter::GetPLLAnimInstance() const
{
	return Cast<UPLLAnimInstance>(FindComponentByClass<USkeletalMeshComponent>()->GetAnimInstance());
}
