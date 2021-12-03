#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"

#include "Variables/AtomFColorVariable.h"
#include "AtomFColorVariableFactoryNew.generated.h"

/** 
* Implements a factory for UAtomFColorVariable objects. 
*/
UCLASS(hidecategories = Object)
class UAtomFColorVariableFactoryNew
: public UFactory
{
	GENERATED_UCLASS_BODY()
public:
	//~ UFactory Interface

	virtual UObject* FactoryCreateNew(UClass*           InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context,
	                                  FFeedbackContext* Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
};


inline UAtomFColorVariableFactoryNew::UAtomFColorVariableFactoryNew(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UAtomFColorVariable::StaticClass();
	bCreateNew     = true;
	bEditAfterNew  = true;
}


inline UObject* UAtomFColorVariableFactoryNew::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                                                UObject* Context,
                                                                FFeedbackContext* Warn)
{
	return NewObject<UAtomFColorVariable>(InParent, InClass, InName, Flags);
}


inline bool UAtomFColorVariableFactoryNew::ShouldShowInNewMenu() const
{
	return true;
}
