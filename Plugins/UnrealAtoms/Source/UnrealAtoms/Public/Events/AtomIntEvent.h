// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AtomBaseEvent.h"
#include "AtomIntEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAtomIntEvent, int, Param);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAtomIntPairEvent, int, Param1, int, Param2);

UCLASS(BlueprintType)
class UNREALATOMS_API UAtomIntEvent : public UAtomBaseEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAtomIntEvent Event;
};

UCLASS(BlueprintType)
class UNREALATOMS_API UAtomIntPairEvent : public UAtomBasePairEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAtomIntPairEvent Event;
};
