#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"

#include "Variables/AtomIntVariable.h"
#include "AtomIntVariableFactoryNew.generated.h"

/** 
* Implements a factory for UAtomIntVariable objects. 
*/
UCLASS(hidecategories = Object)
class UAtomIntVariableFactoryNew
    : public UFactory
{
	GENERATED_UCLASS_BODY()
public:
	//~ UFactory Interface

	virtual UObject *FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags, UObject *Context,
						    FFeedbackContext *Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
};

inline UAtomIntVariableFactoryNew::UAtomIntVariableFactoryNew(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
	SupportedClass = UAtomIntVariable::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

inline UObject *UAtomIntVariableFactoryNew::FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags,
										 UObject *Context,
										 FFeedbackContext *Warn)
{
	return NewObject<UAtomIntVariable>(InParent, InClass, InName, Flags);
}

inline bool UAtomIntVariableFactoryNew::ShouldShowInNewMenu() const
{
	return true;
}