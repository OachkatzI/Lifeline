// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AtomBaseEvent.h"
#include "Engine/DataAsset.h"
#include "AtomFVectorEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAtomFVectorEvent, FVector, Param);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAtomFVectorPairEvent, FVector, Param1, FVector, Param2);

UCLASS(BlueprintType)
class UNREALATOMS_API UAtomFVectorEvent : public UAtomBaseEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAtomFVectorEvent Event;
};

UCLASS(BlueprintType)
class UNREALATOMS_API UAtomFVectorPairEvent : public UAtomBasePairEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAtomFVectorPairEvent Event;
};
