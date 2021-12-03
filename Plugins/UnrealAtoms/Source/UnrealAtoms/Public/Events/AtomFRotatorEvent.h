// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AtomBaseEvent.h"
#include "Engine/DataAsset.h"
#include "AtomFRotatorEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAtomFRotatorEvent, FRotator, Param);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAtomFRotatorPairEvent, FRotator, Param1, FRotator, Param2);

UCLASS(BlueprintType)
class UNREALATOMS_API UAtomFRotatorEvent : public UAtomBaseEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAtomFRotatorEvent Event;
};

UCLASS(BlueprintType)
class UNREALATOMS_API UAtomFRotatorPairEvent : public UAtomBasePairEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAtomFRotatorPairEvent Event;
};
