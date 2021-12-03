// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PLLRopeAnchor.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "PLLRope.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRopeAttached);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRopeReleased, float, ReleaseAngle);


struct FRopeAttachmentParams
{
	FRopeAttachmentParams(AActor* AActorToAttachTo, const FVector FvPositionToAttachTo, const float FNewRopeLength)
	{
		ActorToAttachTo    = AActorToAttachTo;
		PositionToAttachTo = FvPositionToAttachTo;
		NewRopeLength      = FNewRopeLength;
	}


	AActor* ActorToAttachTo;
	FVector PositionToAttachTo;
	float   NewRopeLength;
};


class UCableComponent;
class UPLLRopeSetting;
class APLLCharacter;


UCLASS()
class PROJECTLIFELINE_API APLLRope : public AActor
{
	GENERATED_BODY()

public:

	APLLRope();

	void Tick(float DeltaSeconds) override;

	void Attach(bool bUseMainAnchor, FRopeAttachmentParams Params);

	void Release();

	UPLLRopeSetting& GetRopeSettings() const { return *Settings; }

	float GetRopeLength() const { return RopeLength; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool    GetIsAttached() const { return bIsAttached; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetCurrentSwingPoint() const { return RopeAnchor->GetActorLocation(); }

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnRopeAttached OnRopeAttached;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnRopeReleased OnRopeReleased;

	float GetCurrentSwingAngle() const { return CurrentSwingAngle; }

	// Value between 0 (outside tolerance threshold) and 1 (perfect angle)
	UFUNCTION(BlueprintCallable)
	float GetCurrentSwingAnglePerfectionScale() const { return CurrentSwingAnglePerfectionScale; }


protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cable")
	UCableComponent* Cable;

	UPROPERTY(BlueprintReadOnly)
	APLLCharacter* KaiCharacter;
	
	UPROPERTY(EditDefaultsOnly, Category="RopeAnchorBlueprint")
	TSubclassOf<APLLRopeAnchor> AnchorBluePrint;

	UPROPERTY(EditDefaultsOnly, Category="RopeSettings")
	UPLLRopeSetting* Settings;

	float CalculateReleaseAngle();

	float CurrentSwingAngle;

	float CurrentSwingAnglePerfectionScale;

private:

	UPROPERTY()
	APLLRopeAnchor* RopeAnchor;

	float RopeLength;

	bool bCanHook;

	bool bIsAttached;
};
