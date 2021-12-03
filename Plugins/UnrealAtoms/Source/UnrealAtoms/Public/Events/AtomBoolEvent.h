// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AtomBaseEvent.h"
#include "AtomBoolEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAtomBoolEvent, bool, Param);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAtomBoolPairEvent, bool, Param1, bool, Param2);

UCLASS(BlueprintType)
class UNREALATOMS_API UAtomBoolEvent : public UAtomBaseEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAtomBoolEvent Event;
};

UCLASS(BlueprintType)
class UNREALATOMS_API UAtomBoolPairEvent : public UAtomBasePairEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAtomBoolPairEvent Event;
};
