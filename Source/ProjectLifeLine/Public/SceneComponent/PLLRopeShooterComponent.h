// Project LifeLine

#pragma once

#include "CoreMinimal.h"
#include "Actors/PLLRope.h"
#include "Components/SceneComponent.h"
#include "PLLRopeShooterComponent.generated.h"


class UPLLAimAssistantSettings;
class UPLLHitResult;
class APLLCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRopeReady, APLLRope*, NewRope);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrappleBoostCooldownStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrappleBoostCooldownEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemainingGrappleChargesChanged, uint8, NewRemainingGrappleBoostCharges);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTLIFELINE_API UPLLRopeShooterComponent : public USceneComponent
{
	GENERATED_BODY()

public:

	UPLLRopeShooterComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	APLLRope* GetRope() const { return Rope; }


	UFUNCTION(BlueprintCallable, Category = "RopeBlueprint")
	void Fire();

	UFUNCTION(BlueprintCallable, Category = "RopeBlueprint")
	void Release();

	UFUNCTION(BlueprintCallable)
	void TryApplyGrappleBoost();

	float GetRemainingGrappleBoostCooldown() const;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnRopeReady OnRopeReady;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnGrappleBoostCooldownStarted OnGrapplyBoostCooldownStart;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnGrappleBoostCooldownEnded OnGrapplyBoostCooldownEnd;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnRemainingGrappleChargesChanged OnRemainingGrappleChargesChanged;

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UPLLAimAssistantSettings* Settings;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Traces")
	UPLLHitResult* LineTrace;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Traces")
	UPLLHitResult* BubbleTrace;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Traces")
	UPLLHitResult* GravityTrace;


	virtual void BeginPlay() override;

	UFUNCTION()
	void OnGrappleCooldownEnd();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	TEnumAsByte<ETraceTypeQuery> TraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	TArray<AActor*> ActorsToIgnore;

	UPROPERTY(EditDefaultsOnly, Category = "General Settings")
	TSubclassOf<APLLRope> RopeClass;

	UPROPERTY()
	APLLRope* Rope;

	UPROPERTY()
	APLLCharacter* KaiCharacter;

	UPROPERTY()
	FTimerHandle GrappleBoostCooldownHandle;

	bool bCanHook;

	bool bIsSwinging;

	uint8 RemainingGrappleBoostCharges;

private:

	UPROPERTY()
	APlayerController* PlayerController;
	UPROPERTY()
	APlayerCameraManager* CameraManager;

	bool    CanShootRope = false;
	FVector RopeTargetPosition;
	FVector RopeTargetNormal;
	UPROPERTY()
	AActor* RopeTargetActor = nullptr;

	FRotator CurrentRotation;
	FRotator TargetRotation;
	float    CachedPitch;
	float    CachedYaw;

	void TickBubbleCursor();

	void TickGravity(float DeltaTime);
};
