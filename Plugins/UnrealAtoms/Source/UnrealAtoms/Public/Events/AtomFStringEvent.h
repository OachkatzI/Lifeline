// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AtomBaseEvent.h"
#include "Engine/DataAsset.h"
#include "AtomFStringEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAtomFStringEvent, FString, Param);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAtomFStringPairEvent, FString, Param1, FString, Param2);

UCLASS(BlueprintType)
class UNREALATOMS_API UAtomFStringEvent : public UAtomBaseEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAtomFStringEvent Event;
};

UCLASS(BlueprintType)
class UNREALATOMS_API UAtomFStringPairEvent : public UAtomBasePairEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAtomFStringPairEvent Event;
};
