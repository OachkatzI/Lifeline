// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AtomBaseVariable.h"
#include "UnrealAtoms/Public/Events/AtomIntEvent.h"
#include "AtomIntVariable.generated.h"

UCLASS(BlueprintType)
class UNREALATOMS_API UAtomIntVariable : public UAtomBaseVariable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Set(int NewValue)
	{
		Value = NewValue;
		// if (OnChanged) { OnChanged->Event.Broadcast(Value); }
	}


	UFUNCTION(BlueprintCallable)
	int Get() { return Value; }


	// UPROPERTY(EditAnywhere)
	// TSoftObjectPtr<UAtomIntEvent> OnChanged;

private:
	UPROPERTY(EditAnywhere)
	int Value;
};
