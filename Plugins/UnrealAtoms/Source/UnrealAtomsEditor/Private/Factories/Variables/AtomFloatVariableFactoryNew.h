#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"
#include "Variables/AtomFloatVariable.h"

#include "AtomFloatVariableFactoryNew.generated.h"

/** 
* Implements a factory for UAtomFloatVariable objects. 
*/
UCLASS(hidecategories = Object)
class UAtomFloatVariableFactoryNew
    : public UFactory
{
	GENERATED_UCLASS_BODY()
public:
	//~ UFactory Interface

	virtual UObject *FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags, UObject *Context,
						    FFeedbackContext *Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
};

inline UAtomFloatVariableFactoryNew::UAtomFloatVariableFactoryNew(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
	SupportedClass = UAtomFloatVariable::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

inline UObject *UAtomFloatVariableFactoryNew::FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags,
										   UObject *Context,
										   FFeedbackContext *Warn)
{
	return NewObject<UAtomFloatVariable>(InParent, InClass, InName, Flags);
}

inline bool UAtomFloatVariableFactoryNew::ShouldShowInNewMenu() const
{
	return true;
}