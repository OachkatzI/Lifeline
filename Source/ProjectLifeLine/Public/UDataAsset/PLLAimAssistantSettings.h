// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PLLAimAssistantSettings.generated.h"

/**
* 
*/
UCLASS(BlueprintType)
class PROJECTLIFELINE_API UPLLAimAssistantSettings : public UDataAsset
{
	GENERATED_BODY()

public:

	//BubbleCursor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Bubble Cursor")
	float BubbleCursorRadius = 10;

	//Gravity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TargetGravity")
	float GravityRotationLerpSpeed = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TargetGravity")
	float GravityRadius = 200;
};
