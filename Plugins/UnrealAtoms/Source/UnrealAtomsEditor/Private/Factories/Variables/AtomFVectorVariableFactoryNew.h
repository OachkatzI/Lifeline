#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"
#include "Variables/AtomFVectorVariable.h"
#include "AtomFVectorVariableFactoryNew.generated.h"

/** 
* Implements a factory for UAtomFVectorVariable objects. 
*/
UCLASS(hidecategories = Object)
class UAtomFVectorVariableFactoryNew
    : public UFactory
{
	GENERATED_UCLASS_BODY()
public:
	//~ UFactory Interface

	virtual UObject *FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags, UObject *Context,
						    FFeedbackContext *Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
};

inline UAtomFVectorVariableFactoryNew::UAtomFVectorVariableFactoryNew(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
	SupportedClass = UAtomFVectorVariable::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

inline UObject *UAtomFVectorVariableFactoryNew::FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags,
										     UObject *Context,
										     FFeedbackContext *Warn)
{
	return NewObject<UAtomFVectorVariable>(InParent, InClass, InName, Flags);
}

inline bool UAtomFVectorVariableFactoryNew::ShouldShowInNewMenu() const
{
	return true;
}