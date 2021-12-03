// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AtomBaseVariable.h"
#include "Events/AtomFVectorEvent.h"

#include "AtomFVectorVariable.generated.h"

UCLASS(BlueprintType)
class UNREALATOMS_API UAtomFVectorVariable : public UAtomBaseVariable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Set(FVector NewValue)
	{
		Value = NewValue;
		// if (OnChanged) { OnChanged->Event.Broadcast(Value); }
	}


	UFUNCTION(BlueprintCallable)
	FVector Get() { return Value; }


	// UPROPERTY(EditAnywhere)
	// TSoftObjectPtr<UAtomFVectorEvent> OnChanged;

private:
	UPROPERTY(EditAnywhere)
	FVector Value;
};
