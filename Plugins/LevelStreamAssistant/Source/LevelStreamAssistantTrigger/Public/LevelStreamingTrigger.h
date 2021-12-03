// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelStreamingTrigger.generated.h"

class UGateComponent;

/**
 * When player enters behind the arrow, specified levels are loaded and unloaded. when player exits behind the arrow, loaded and unloaded levels are reversed.
 */
UCLASS()
class LEVELSTREAMASSISTANTTRIGGER_API ALevelStreamingTrigger : public AActor
{
	GENERATED_BODY()

public:
	ALevelStreamingTrigger();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	TArray<TSoftObjectPtr<UWorld>> LevelsToLoad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	TArray<TSoftObjectPtr<UWorld>> LevelsToUnload;

	/* In two way mode, when player enters behind the arrow, specified levels are loaded or unloaded. when player exits behind the arrow, loaded or unloaded levels are reversed.
In one way mode, player can enter from any direction and specified levels will be loaded or unloaded. (nothing is reversed)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	uint8 bIsTwoWay : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	uint8 bMakeVisibleAfterLoad : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Setup")
	uint8 bShouldBlockOnLoad : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Setup")
	uint8 bShouldBlockOnUnload : 1;

	UGateComponent* GetTriggerComponent() const { return TriggerComponent; }

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UGateComponent* TriggerComponent;

#if WITH_EDITORONLY_DATA
	// Entry direction.
	UPROPERTY()
	class UArrowComponent* ArrowComponent;

	UPROPERTY()
	class UBillboardComponent* BillboardComponent;
#endif

	UFUNCTION()
	void HandleActorEntered(AActor* EnteredActor, UPrimitiveComponent* EnteredComponent);

	UFUNCTION()
	void HandleActorExited(AActor* ExitedActor, UPrimitiveComponent* ExitedComponent);

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SetLevelsToStream(const TArray<TSoftObjectPtr<UWorld>>& InLevelsToLoad, const TArray<TSoftObjectPtr<UWorld>>& InLevelsToUnload);

	void SetLevelToStream(const FName& LevelName, bool bShouldBeLoaded, bool bShouldBeVisible, bool bShouldBlock);
};
