#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"
#include "Variables/AtomBoolVariable.h"

#include "AtomBoolVariableFactoryNew.generated.h"

/** 
* Implements a factory for UAtomBoolVariable objects. 
*/
UCLASS(hidecategories = Object)
class UAtomBoolVariableFactoryNew
    : public UFactory
{
	GENERATED_UCLASS_BODY()
public:
	//~ UFactory Interface

	virtual UObject *FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags, UObject *Context,
						    FFeedbackContext *Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
};

inline UAtomBoolVariableFactoryNew::UAtomBoolVariableFactoryNew(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
	SupportedClass = UAtomBoolVariable::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

inline UObject *UAtomBoolVariableFactoryNew::FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags,
										  UObject *Context,
										  FFeedbackContext *Warn)
{
	return NewObject<UAtomBoolVariable>(InParent, InClass, InName, Flags);
}

inline bool UAtomBoolVariableFactoryNew::ShouldShowInNewMenu() const
{
	return true;
}