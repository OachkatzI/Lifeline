// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PLLCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "PLLPlayerController.generated.h"


class UPLLCharacterMovementComponent;
class APLLCharacter;
class UPLLParkourSystemComponent;


UCLASS()
class PROJECTLIFELINE_API APLLPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	APLLPlayerController();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bInvertCameraTurn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bInvertCameraUp;

	UPROPERTY(EditAnywhere)
	float TimeDeadBeforeRespawn;

	void BeginPlay() override;

public:
	void Tick(float DeltaSeconds) override;

	FORCEINLINE APLLCameraManager* GetPllCameraManager() const { return Cast<APLLCameraManager>(PlayerCameraManager); }

	// UFUNCTION(BlueprintCallable)
	// FVector GetPlayerCameraRawInput() const { return RotationInput.Vector(); }


	UFUNCTION(BlueprintCallable)
	void AddForwardInput(float Value);

	UFUNCTION(BlueprintCallable)
	void AddRightInput(float Value);


	UFUNCTION(BlueprintCallable)
	void SetInputReceiveModeToAll();

	UFUNCTION(BlueprintCallable)
	void SetInputReceiveModeToNone();

	UFUNCTION(BlueprintCallable)
	void DisableInputMode();

	UFUNCTION(BlueprintCallable)
	void EnableInputMode();

	UFUNCTION(BlueprintCallable)
	void OnRespawn();

	void SwitchCameraView();

	void SwitchToCamera(CameraViewMode NewCameraViewMode);

	void SetupInput();

	//Properties
	UPROPERTY(BlueprintReadWrite)
	FVector CameraRawInput;

	UPROPERTY(BlueprintReadWrite)
	float MovementActionTimePressedDown = -1;

	UPROPERTY(BlueprintReadWrite)
	bool bCanWalkRight = true;

	UPROPERTY(BlueprintReadWrite)
	bool bCanWalkForward = true;

	UPROPERTY(BlueprintReadWrite)
	bool bCanPerformJump = true;

	UPROPERTY(BlueprintReadWrite)
	bool bCanPerformUpAction = true;

	UPROPERTY(BlueprintReadWrite)
	bool bCanPerformDownAction = true;

	// UPROPERTY(BlueprintReadWrite)
	// bool bCanPerformMovementAction = true;

	UPROPERTY(BlueprintReadWrite)
	bool bCanLookAround = true;

	UPROPERTY(BlueprintReadWrite)
	bool bCanFireRope = true;


	UPROPERTY(BlueprintReadOnly)
	FVector AccumulatedDirectionalInput;

	UPROPERTY(BlueprintReadOnly)
	FVector LastDirectionalInput;


private:

	UPROPERTY()
	APLLCharacter* KaiCharacter;

	UPROPERTY()
	UPLLCharacterMovementComponent* KaiMovComponent;

	UPROPERTY()
	APLLCameraManager* CameraManager;
	bool               bIsCameraInputEnabled = true;

	// UPROPERTY()
	// UPLLParkourSystemComponent* ParkourSystemComp;

	void PropagateInputVector();

	UFUNCTION()
	void HandleOnDeath();

	FTimerHandle TimeDeadBeforeRespawnHandle;

	FRotator DefaultMeshRelativeRotation;

	FVector DefaultMeshRelativeLocation;


	bool bCacheCanWalkRight         = true;
	bool bCacheCanWalkForward       = true;
	bool bCacheCanPerformJump       = true;
	bool bCacheCanPerformUpAction   = true;
	bool bCacheCanPerformDownAction = true;
	bool bCacheCanLookAround        = true;
	bool bCacheCanFireRope          = true;
};
