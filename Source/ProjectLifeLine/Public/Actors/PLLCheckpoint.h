// Project LifeLine

#pragma once

#include "CoreMinimal.h"

#include "Characters/PLLCharacter.h"
#include "Engine/TriggerBox.h"
#include "PLLCheckpoint.generated.h"

/**
 * 
 */

class APLLGameModePlay;
UCLASS()
class PROJECTLIFELINE_API APLLCheckpoint : public ATriggerBox
{
	GENERATED_BODY()
	
public:
	APLLCheckpoint();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    FString CheckpointTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
	FRotator LookAtRotation;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
	bool FadeOnEnter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
	bool DisableInputOnEnter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Start Checkpoint")
	bool IsStartCheckpoint;
	
	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION(BlueprintImplementableEvent)
	void OnResetToCheckopoint();

	UFUNCTION(BlueprintImplementableEvent)
    void OnCheckpointEntered();

	//gets called when player enters checkpoint
	UFUNCTION(BlueprintImplementableEvent)
	void CheckpointAction();

	virtual void Tick(float DeltaSeconds) override;

protected:
	
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void Teleport(APLLCharacter* TargetCharacter, APLLCheckpoint* TargetCheckpoint, FRotator TargetRotation);

	UFUNCTION(BlueprintCallable)
	void ResetMomentum(APLLCharacter* TargetCharacter);
	
private:
	
	APLLGameModePlay* MyGameMode;	
};
