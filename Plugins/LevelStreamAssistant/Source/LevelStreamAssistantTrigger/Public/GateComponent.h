// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GateComponent.generated.h"

// Provide Enter and Exit triggers behind the gate.
UCLASS( ClassGroup=("Custom"), meta=(BlueprintSpawnableComponent) )
class LEVELSTREAMASSISTANTTRIGGER_API UGateComponent : public UBoxComponent
{
	GENERATED_BODY()

public:	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FActorEntered, AActor*, EnteredActor, UPrimitiveComponent*, EnteredComponent);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FActorExited, AActor*, ExitedActor, UPrimitiveComponent*, ExitedComponent);

	UGateComponent();

	UPROPERTY(BlueprintAssignable, Category = "Gate")
	FActorEntered OnActorEntered;

	UPROPERTY(BlueprintAssignable, Category = "Gate")
	FActorExited OnActorExited;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

private:
	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
