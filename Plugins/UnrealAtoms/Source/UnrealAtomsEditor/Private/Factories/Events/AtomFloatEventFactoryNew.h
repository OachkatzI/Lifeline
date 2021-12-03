#pragma once
#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"

#include "Events/AtomFloatEvent.h"
#include "AtomFloatEventFactoryNew.generated.h"

/** 
* Implements a factory for UAtomFloatEvent objects. 
*/
UCLASS(hidecategories = Object)
class UAtomFloatEventFactoryNew
	: public UFactory
{
	GENERATED_UCLASS_BODY()
public:
	//~ UFactory Interface

	virtual UObject* FactoryCreateNew(UClass*           InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context,
	                                  FFeedbackContext* Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
};


inline UAtomFloatEventFactoryNew::UAtomFloatEventFactoryNew(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UAtomFloatEvent::StaticClass();
	bCreateNew     = true;
	bEditAfterNew  = true;
}


inline UObject* UAtomFloatEventFactoryNew::FactoryCreateNew(UClass*           InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                                            UObject*          Context,
                                                            FFeedbackContext* Warn)
{
	return NewObject<UAtomFloatEvent>(InParent, InClass, InName, Flags);
}


inline bool UAtomFloatEventFactoryNew::ShouldShowInNewMenu() const
{
	return true;
}
