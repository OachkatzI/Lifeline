// Project LifeLine

#pragma once

#include "CoreMinimal.h"

#include "Actors/PLLCheckpoint.h"
#include "GameFramework/SaveGame.h"
#include "PLLSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTLIFELINE_API UPLLSaveGame : public USaveGame
{
	GENERATED_BODY()
	//https://docs.unrealengine.com/en-US/InteractiveExperiences/SaveGame/index.html

public:
	UPLLSaveGame();

	UPROPERTY(VisibleAnywhere, Category = SaveGame)
	FString PlayerName;

	UPROPERTY(VisibleAnywhere, Category = SaveGame)
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = SaveGame)
	uint32 UserIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= SaveGame)
	APLLCheckpoint* LastCheckpoint;
};
