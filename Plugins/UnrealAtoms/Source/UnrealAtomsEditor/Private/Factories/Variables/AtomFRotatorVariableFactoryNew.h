#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"
#include "Variables/AtomFRotatorVariable.h"

#include "AtomFRotatorVariableFactoryNew.generated.h"

/** 
* Implements a factory for UAtomFRotatorVariable objects. 
*/
UCLASS(hidecategories = Object)
class UAtomFRotatorVariableFactoryNew
    : public UFactory
{
	GENERATED_UCLASS_BODY()
public:
	//~ UFactory Interface

	virtual UObject *FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags, UObject *Context,
						    FFeedbackContext *Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
};

inline UAtomFRotatorVariableFactoryNew::UAtomFRotatorVariableFactoryNew(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
	SupportedClass = UAtomFRotatorVariable::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

inline UObject *UAtomFRotatorVariableFactoryNew::FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags,
											UObject *Context,
											FFeedbackContext *Warn)
{
	return NewObject<UAtomFRotatorVariable>(InParent, InClass, InName, Flags);
}

inline bool UAtomFRotatorVariableFactoryNew::ShouldShowInNewMenu() const
{
	return true;
}