// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Assistant/SimpleClassFilter.h"

bool FSimpleClassFilter::IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs)
{
	return InClass && !InClass->HasAnyClassFlags(EClassFlags::CLASS_Abstract) && // don't allow abstract classes
		InFilterFuncs->IfInChildOfClassesSet(AllowedClasses, InClass) == EFilterReturn::Passed;
}

bool FSimpleClassFilter::IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs)
{
	return !InUnloadedClassData->HasAnyClassFlags(EClassFlags::CLASS_Abstract) && // don't allow abstract classes
		InFilterFuncs->IfInChildOfClassesSet(AllowedClasses, InUnloadedClassData) == EFilterReturn::Passed;
}
