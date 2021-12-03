#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"

#include "Events/AtomFStringEvent.h"
#include "AtomFStringEventFactoryNew.generated.h"

/** 
* Implements a factory for UAtomFStringEvent objects. 
*/
UCLASS(hidecategories = Object)
class UAtomFStringEventFactoryNew
    : public UFactory
{
	GENERATED_UCLASS_BODY()
public:
	//~ UFactory Interface

	virtual UObject *FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags, UObject *Context,
						    FFeedbackContext *Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
};

inline UAtomFStringEventFactoryNew::UAtomFStringEventFactoryNew(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
	SupportedClass = UAtomFStringEvent::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

inline UObject *UAtomFStringEventFactoryNew::FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags,
										  UObject *Context,
										  FFeedbackContext *Warn)
{
	return NewObject<UAtomFStringEvent>(InParent, InClass, InName, Flags);
}

inline bool UAtomFStringEventFactoryNew::ShouldShowInNewMenu() const
{
	return true;
}