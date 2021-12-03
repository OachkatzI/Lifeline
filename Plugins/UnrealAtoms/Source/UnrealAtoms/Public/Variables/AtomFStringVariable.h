// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AtomBaseVariable.h"
#include "Events/AtomFStringEvent.h"

#include "AtomFStringVariable.generated.h"

UCLASS(BlueprintType)
class UNREALATOMS_API UAtomFStringVariable : public UAtomBaseVariable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Set(FString NewValue)
	{
		Value = NewValue;
		// if (OnChanged) { OnChanged->Event.Broadcast(Value); }
	}


	UFUNCTION(BlueprintCallable)
	FString Get() { return Value; }

	
	// UPROPERTY(EditAnywhere)
	// TSoftObjectPtr<UAtomFStringEvent> OnChanged;

private:
	UPROPERTY(EditAnywhere)
	FString Value;
};
