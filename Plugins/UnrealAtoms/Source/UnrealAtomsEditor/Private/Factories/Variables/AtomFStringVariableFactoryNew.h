#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"

#include "Variables/AtomFStringVariable.h"
#include "AtomFStringVariableFactoryNew.generated.h"

/** 
* Implements a factory for UAtomFStringVariable objects. 
*/
UCLASS(hidecategories = Object)
class UAtomFStringVariableFactoryNew
    : public UFactory
{
	GENERATED_UCLASS_BODY()
public:
	//~ UFactory Interface

	virtual UObject *FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags, UObject *Context,
						    FFeedbackContext *Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
};

inline UAtomFStringVariableFactoryNew::UAtomFStringVariableFactoryNew(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
	SupportedClass = UAtomFStringVariable::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

inline UObject *UAtomFStringVariableFactoryNew::FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags,
										     UObject *Context,
										     FFeedbackContext *Warn)
{
	return NewObject<UAtomFStringVariable>(InParent, InClass, InName, Flags);
}

inline bool UAtomFStringVariableFactoryNew::ShouldShowInNewMenu() const
{
	return true;
}