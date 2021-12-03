#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"

#include "Events/AtomFVectorEvent.h"
#include "AtomFVectorEventFactoryNew.generated.h"

/** 
* Implements a factory for UAtomFVectorEvent objects. 
*/
UCLASS(hidecategories = Object)
class UAtomFVectorEventFactoryNew
    : public UFactory
{
	GENERATED_UCLASS_BODY()
public:
	//~ UFactory Interface

	virtual UObject *FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags, UObject *Context,
						    FFeedbackContext *Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
};

inline UAtomFVectorEventFactoryNew::UAtomFVectorEventFactoryNew(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
	SupportedClass = UAtomFVectorEvent::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

inline UObject *UAtomFVectorEventFactoryNew::FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags,
										  UObject *Context,
										  FFeedbackContext *Warn)
{
	return NewObject<UAtomFVectorEvent>(InParent, InClass, InName, Flags);
}

inline bool UAtomFVectorEventFactoryNew::ShouldShowInNewMenu() const
{
	return true;
}