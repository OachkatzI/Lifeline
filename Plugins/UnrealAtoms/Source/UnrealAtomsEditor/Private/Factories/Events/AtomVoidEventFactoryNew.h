#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"

#include "Events/AtomVoidEvent.h"
#include "AtomVoidEventFactoryNew.generated.h"

/** 
* Implements a factory for UAtomIntEvent objects. 
*/
UCLASS(hidecategories = Object)
class UAtomVoidEventFactoryNew
	: public UFactory
{
	GENERATED_UCLASS_BODY()
public:
	//~ UFactory Interface

	virtual UObject* FactoryCreateNew(UClass*           InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context,
	                                  FFeedbackContext* Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
};


inline UAtomVoidEventFactoryNew::UAtomVoidEventFactoryNew(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UAtomVoidEvent::StaticClass();
	bCreateNew     = true;
	bEditAfterNew  = true;
}


inline UObject* UAtomVoidEventFactoryNew::FactoryCreateNew(UClass*           InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                                           UObject*          Context,
                                                           FFeedbackContext* Warn)
{
	return NewObject<UAtomVoidEvent>(InParent, InClass, InName, Flags);
}


inline bool UAtomVoidEventFactoryNew::ShouldShowInNewMenu() const
{
	return true;
}
