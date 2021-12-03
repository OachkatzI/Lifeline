#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"

#include "Events/AtomIntEvent.h"
#include "AtomIntEventFactoryNew.generated.h"

/** 
* Implements a factory for UAtomIntEvent objects. 
*/
UCLASS(hidecategories = Object)
class UAtomIntEventFactoryNew
    : public UFactory
{
	GENERATED_UCLASS_BODY()
public:
	//~ UFactory Interface

	virtual UObject *FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags, UObject *Context,
						    FFeedbackContext *Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
};

inline UAtomIntEventFactoryNew::UAtomIntEventFactoryNew(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
	SupportedClass = UAtomIntEvent::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

inline UObject *UAtomIntEventFactoryNew::FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags,
									    UObject *Context,
									    FFeedbackContext *Warn)
{
	return NewObject<UAtomIntEvent>(InParent, InClass, InName, Flags);
}

inline bool UAtomIntEventFactoryNew::ShouldShowInNewMenu() const
{
	return true;
}