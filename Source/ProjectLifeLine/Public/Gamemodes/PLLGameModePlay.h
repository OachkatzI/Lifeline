// Project LifeLine

#pragma once

#include "CoreMinimal.h"


#include "PLLSaveGame.h"
#include "Characters/PLLCharacter.h"
#include "GameFramework/GameModeBase.h"

#include "PLLGameModePlay.generated.h"

class APLLCheckpoint;

UCLASS()
class PROJECTLIFELINE_API APLLGameModePlay : public AGameModeBase
{
	GENERATED_BODY()

public:

	APLLGameModePlay();
	
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintCallable)
    void CompleteMission();

    UFUNCTION(BlueprintCallable)
    void ResetToLastCheckpoint();

	void SetLastCheckpoint(APLLCheckpoint* LastCheckpoint);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category= "Checkpoint")
	APLLCheckpoint* DefaultStartCheckpoint;

	UPROPERTY(BlueprintReadWrite, Category = "Level")
	FString CurrentLevelName;

	UPROPERTY(BlueprintReadWrite)
	bool DoesSaveGameExist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Point")
	bool StartFromEditorLocation;
	
protected:
	
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetInputMode();

	UFUNCTION(BlueprintImplementableEvent)
    void SetDefaultStartCheckpoint();
	
	void InitSafeGame();
	
	void InitLoadGame();
	
	void SafeGameLogic();
	
	void LoadGameLogic(float dt);
	
private:

	APLLCheckpoint* MyLastCheckpoint;

	APLLCharacter* KaiCharacter;

	FVector MyStartLocation;

	bool CanSaveGame;

	float SaveGameTimer;

	bool IsPreparingSave;
	bool hasCompletedLoadGame;
	float loadingSequenceTimer;
	bool isAssemblingSaveGameData;
	bool GameLoaded;

	FString CurrentSaveSlotName;
};
