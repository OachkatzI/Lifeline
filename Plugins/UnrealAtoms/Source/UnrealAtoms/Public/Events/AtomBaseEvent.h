// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AtomBaseEvent.generated.h"

UCLASS(Abstract, NotPlaceable)
class UNREALATOMS_API UAtomBaseEvent : public UDataAsset
{
	GENERATED_BODY()
};

UCLASS(Abstract)
class UNREALATOMS_API UAtomBasePairEvent : public UAtomBaseEvent
{
	GENERATED_BODY()
};
