#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"
#include "Events/AtomBoolEvent.h"
#include "AtomBoolEventFactoryNew.generated.h"


/** 
* Implements a factory for UAtomBoolEvent objects. 
*/
UCLASS(hidecategories=Object)
class UAtomBoolEventFactoryNew
	: public UFactory
{
	GENERATED_UCLASS_BODY()
public:


	//~ UFactory Interface 

	virtual UObject* FactoryCreateNew(UClass*           InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context,
	                                  FFeedbackContext* Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
};



inline UAtomBoolEventFactoryNew::UAtomBoolEventFactoryNew(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UAtomBoolEvent::StaticClass();
	bCreateNew     = true;
	bEditAfterNew  = true;
}

inline UObject* UAtomBoolEventFactoryNew::FactoryCreateNew(UClass*           InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                                           UObject*          Context,
                                                           FFeedbackContext* Warn)
{
	return NewObject<UAtomBoolEvent>(InParent, InClass, InName, Flags);
}


inline bool UAtomBoolEventFactoryNew::ShouldShowInNewMenu() const
{
	return true;
}
