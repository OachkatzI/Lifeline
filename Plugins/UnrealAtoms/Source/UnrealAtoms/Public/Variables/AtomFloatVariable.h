// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AtomBaseVariable.h"
#include "Events/AtomFloatEvent.h"

#include "AtomFloatVariable.generated.h"

UCLASS(BlueprintType)
class UNREALATOMS_API UAtomFloatVariable : public UAtomBaseVariable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Set(float NewValue)
	{
		Value = NewValue;
		// if (OnChanged) { OnChanged->Event.Broadcast(Value); }
	}


	UFUNCTION(BlueprintCallable)
	float Get() { return Value; }

	//
	// UPROPERTY(EditAnywhere)
	// TSoftObjectPtr<UAtomFloatEvent> OnChanged;

private:
	UPROPERTY(EditAnywhere)
	float Value;
};
