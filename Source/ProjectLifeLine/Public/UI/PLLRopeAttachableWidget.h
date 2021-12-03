// Project LifeLine

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UINavigation/Public/UINavWidget.h"
#include "PLLRopeAttachableWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTLIFELINE_API UPLLRopeAttachableWidget : public UUINavWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void RopeAttachable(bool isAttachable);
};
