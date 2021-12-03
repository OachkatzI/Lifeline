// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AtomBaseEvent.h"
#include "AtomVoidEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAtomVoidEvent);


UCLASS(BlueprintType)
class UNREALATOMS_API UAtomVoidEvent : public UAtomBaseEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAtomVoidEvent Event;
};
