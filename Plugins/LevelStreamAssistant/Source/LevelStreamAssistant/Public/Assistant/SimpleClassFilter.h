// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ClassViewerFilter.h"

class FClassViewerFilterFuncs;
class IUnloadedBlueprintData;

class FSimpleClassFilter : public IClassViewerFilter
{
public:
	FSimpleClassFilter(const TSet<const UClass*>& InAllowedClasses)
		: AllowedClasses(InAllowedClasses) { }

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override;
	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override;

private:
	TSet<const UClass*> AllowedClasses;
};