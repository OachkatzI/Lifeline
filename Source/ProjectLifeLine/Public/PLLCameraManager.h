// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "UDataAsset/LandingSettings.h"
#include "PLLCameraManager.generated.h"


UENUM()
enum CameraViewMode
{
	FirstPerson = 0,
	ThirdPerson = 1,
	COUNT = 2
};


class UCameraComponent;
class APLLCharacter;


UCLASS()
class PROJECTLIFELINE_API APLLCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	UPROPERTY()
	UCameraComponent* FirstPersonCamera;

	UPROPERTY()
	UCameraComponent* ThirdPersonCamera;
	
public:
	
	void SwitchCameraView();

	void ApplyFirstPersonView();

	void ApplyThirdPersonView();

	UFUNCTION(BlueprintCallable)
	void Init(APawn* Pawn);

protected:

	CameraViewMode CurrentViewMode;

	APLLCharacter* KaiCharacter;

	UPROPERTY(EditAnywhere, Category = "Landing")
	ULandingSettings* LandingSettings;
	
	UFUNCTION()
	void HandleLandingCamera(bool bWasPerfect, float LandingScore);

};
