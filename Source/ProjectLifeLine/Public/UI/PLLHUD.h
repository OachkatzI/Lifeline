// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"

#include "PLLRopeAttachableWidget.h"
#include "GameFramework/HUD.h"
#include "PLLHUD.generated.h"

class UPLLCharacterSensorComponent;

UCLASS()
class APLLHUD : public AHUD
{
	GENERATED_BODY()

public:
	APLLHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	UPROPERTY(BlueprintReadWrite)
	UPLLRopeAttachableWidget* CrosshairWidget;
	
	void SetRopeAttachable(bool IsAttachable);

private:
	bool RopeIsAttachable = false;
};

