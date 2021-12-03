// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AtomBaseEvent.h"
#include "Engine/DataAsset.h"
#include "AtomFloatEvent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAtomFloatEvent, float, Param);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAtomFloatPairEvent, float, Param1, float, Param2);

UCLASS(BlueprintType)
class UNREALATOMS_API UAtomFloatEvent : public UAtomBaseEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAtomFloatEvent Event;
};

UCLASS(BlueprintType)
class UNREALATOMS_API UAtomFloatPairEvent : public UAtomBasePairEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAtomFloatPairEvent Event;
};
