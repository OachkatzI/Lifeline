// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AtomBaseVariable.h"
#include "Events/AtomFRotatorEvent.h"

#include "AtomFRotatorVariable.generated.h"

UCLASS(BlueprintType)
class UNREALATOMS_API UAtomFRotatorVariable : public UAtomBaseVariable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Set(FRotator NewValue)
	{
		Value = NewValue;
		// if (OnChanged) { OnChanged->Event.Broadcast(Value); }
	}


	UFUNCTION(BlueprintCallable)
	FRotator Get() { return Value; }

	
	// UPROPERTY(EditAnywhere)
	// TSoftObjectPtr<UAtomFRotatorEvent> OnChanged;

private:
	UPROPERTY(EditAnywhere)
	FRotator Value;
};
