// Project LifeLine

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PLLFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTLIFELINE_API UPLLFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "IsGamepadConnected"), Category = "System Information")
	static bool IsGamePadConnected();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetProjectVersion"), Category="PojectSettings")
	static void GetProjectVersion(FString& Version);
};
