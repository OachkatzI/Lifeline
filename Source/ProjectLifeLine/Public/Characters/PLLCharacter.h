// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PLLCharacter.generated.h"


class UPLLAnimInstance;
class APLLPlayerController;
class UInputComponent;
class UCameraComponent;
class USpringArmComponent;
class APLLRopeShooter;
class UPLLCharacterMovementComponent;
class UBoxComponent;
class UPLLBulletTimeComponent;
class UPLLRopeShooterComponent;
class UPLLParkourSystemComponent;


UCLASS(config=Game)
class APLLCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	APLLCharacter()
	{
	}


	APLLCharacter(const class FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(EditAnywhere)
	float CameraThreshold = 10;


	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;

	void AddControllerYawInput(float Val) override;
	
	FORCEINLINE UCameraComponent*         GetFirstPersonCamera() const { return FirstPersonCamera; }
	FORCEINLINE USpringArmComponent*      GetThirdPersonCameraBoom() const { return ThirdPersonCameraBoom; }
	FORCEINLINE UCameraComponent*         GetThirdPersonCamera() const { return ThirdPersonCamera; }
	FORCEINLINE UPLLRopeShooterComponent* GetRopeShooterComp() const { return RopeShooterComponent; }

	void  PreUpdateCamera(float DeltaTime);
	float CameraProcessPitch(float Input);
	float CameraProcessYaw(float Input);


	UFUNCTION(BlueprintCallable)
	APLLPlayerController* GetPLLController() const;

	UFUNCTION(BlueprintCallable)
	UPLLCharacterMovementComponent* GetPLLMovementComp() const;

	UFUNCTION(BlueprintCallable)
	UPLLAnimInstance* GetPLLAnimInstance() const;

	// UFUNCTION(BlueprintCallable)
	// UPLLParkourSystemComponent* GetPLLParkourSystemComp() const;

	UPROPERTY(BlueprintReadOnly)
	FRotator CameraLocalRotation;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* ThirdPersonCameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* ThirdPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UPLLBulletTimeComponent* BulletTimeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UPLLRopeShooterComponent* RopeShooterComponent;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UPLLCharacterMovementComponent* KaiMovementComp;

	// UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	// UPLLParkourSystemComponent* KaiParkourSystemComp;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	APLLPlayerController* KaiController;
};
