// Fill out your copyright notice in the Description page of Project Settings.


#include "PLLCameraManager.h"
#include "Characters/PLLCharacter.h"
#include "Characters/Movement/PLLCharacterMovementComponent.h"
#include "Camera/CameraComponent.h"


void APLLCameraManager::Init(APawn* Pawn)
{
	KaiCharacter = Cast<APLLCharacter>(Pawn);
	check(KaiCharacter);

	FirstPersonCamera = KaiCharacter->GetFirstPersonCamera();
	ThirdPersonCamera = KaiCharacter->GetThirdPersonCamera();

	KaiCharacter->GetPLLMovementComp()->OnPerformLanding.AddDynamic(this, &APLLCameraManager::HandleLandingCamera);

	check(LandingSettings)
}


void APLLCameraManager::SwitchCameraView()
{
	uint8 NextViewMode = (static_cast<uint8>(CurrentViewMode) + 1) % CameraViewMode::COUNT;
	CurrentViewMode = static_cast<CameraViewMode>(NextViewMode);

	if (CurrentViewMode == FirstPerson)
	{
		ApplyFirstPersonView();
	}
	else if (CurrentViewMode == ThirdPerson)
	{
		ApplyThirdPersonView();
	}
}


void APLLCameraManager::ApplyFirstPersonView()
{
	FirstPersonCamera->SetActive(true);
	ThirdPersonCamera->SetActive(false);
}


void APLLCameraManager::ApplyThirdPersonView()
{
	ThirdPersonCamera->SetActive(true);
	FirstPersonCamera->SetActive(false);
}


void APLLCameraManager::HandleLandingCamera(bool bWasPerfect, float LandingScore)
{
	if (bWasPerfect)
	{
		PlayCameraAnim(LandingSettings->PerfectLandingAnim, KaiCharacter->GetActorTimeDilation());
	}
}
