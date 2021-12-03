// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AtomBaseVariable.h"
#include "Events/AtomBoolEvent.h"

#include "AtomBoolVariable.generated.h"

UCLASS(BlueprintType)
class UNREALATOMS_API UAtomBoolVariable : public UAtomBaseVariable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Set(bool NewValue)
	{
		Value = NewValue;
		// if (OnChanged) { OnChanged->Event.Broadcast(Value); }
	}


	UFUNCTION(BlueprintCallable)
	bool Get() { return Value; }


	// UPROPERTY(EditAnywhere)
	// TSoftObjectPtr<UAtomBoolEvent> OnChanged;

private:
	UPROPERTY(EditAnywhere)
	bool Value;
};
