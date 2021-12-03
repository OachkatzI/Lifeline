#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"

#include "Events/AtomFRotatorEvent.h"
#include "AtomFRotatorEventFactoryNew.generated.h"

/** 
* Implements a factory for UAtomFRotatorEvent objects. 
*/
UCLASS(hidecategories = Object)
class UAtomFRotatorEventFactoryNew
    : public UFactory
{
	GENERATED_UCLASS_BODY()
public:
	//~ UFactory Interface

	virtual UObject *FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags, UObject *Context,
						    FFeedbackContext *Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
};

inline UAtomFRotatorEventFactoryNew::UAtomFRotatorEventFactoryNew(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
	SupportedClass = UAtomFRotatorEvent::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

inline UObject *UAtomFRotatorEventFactoryNew::FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags,
										   UObject *Context,
										   FFeedbackContext *Warn)
{
	return NewObject<UAtomFRotatorEvent>(InParent, InClass, InName, Flags);
}

inline bool UAtomFRotatorEventFactoryNew::ShouldShowInNewMenu() const
{
	return true;
}