// Project LifeLine

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UDataAsset/BulletTimeSettings.h"
#include "PLLBulletTimeComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBulletTimeStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBulletTimeEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBulletTimeReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBulletTimeCooldownStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBulletTimeCooldownEnd);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTLIFELINE_API UPLLBulletTimeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UPLLBulletTimeComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void StartBulletTime();

	UFUNCTION()
	void EndBulletTime();

	UFUNCTION()
	void StopAll();

	// Returns between 1 (cooldown finished) and 0 (just started)
	UFUNCTION(BlueprintCallable)
	float GetCurrentCooldownElapsedRatio() const;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnBulletTimeStart OnBulletTimeStart;
	
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnBulletTimeEnd OnBulletTimeEnd;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnBulletTimeReady OnBulletTimeReady;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnBulletTimeEnd OnBulletTimeCooldownStart;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnBulletTimeReady OnBulletTimeCooldownEnd;
	
protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void ActivateMainBulletTime();

	float GetDilationFromCurve() const;

	UFUNCTION()
	void SetBulletTimeReady();

	UPROPERTY(EditAnywhere, Category = "Time Behaviour")
	UBulletTimeSettings* Settings;

	UPROPERTY()
	UCurveFloat* ActiveCurve;
	
	UPROPERTY()
	FTimerHandle BulletTimerHandle;

	UPROPERTY()
	FTimerHandle BulletCooldownTimerHandle;

};
