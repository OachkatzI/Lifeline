// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Events/AtomBaseEvent.h"

#include "AtomBaseVariable.generated.h"

/**
 * 
 */
UCLASS(NotPlaceable)
class UNREALATOMS_API UAtomBaseVariable : public UDataAsset
{
	GENERATED_BODY()
};


// template <class T>
//
// class IAtomBaseValue 
// {
// protected:
// 	~IAtomBaseValue() = default;
// public:
// 	virtual void Set(T NewValue);
// 	virtual T    Get();
//
// 	TSubclassOf<UAtomBaseEvent> Changed;
// private:
// 	T Value;
// };
//
//
// template <class T>
// void IAtomBaseValue<T>::Set(T NewValue)
// {
// 	Value = NewValue;
// }
//
//
// template <class T>
// T IAtomBaseValue<T>::Get()
// {
// 	return Value;
// }
