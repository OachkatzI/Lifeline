// Project LifeLine

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PLLHitResult.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTLIFELINE_API UPLLHitResult : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	bool bDidHit;

	UPROPERTY(BlueprintReadOnly)
	FHitResult Hit;
};
