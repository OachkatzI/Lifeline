// Project LifeLine

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "UDataAsset/PLLAimAssistantSettings.h"


#include "PLLCharacterSensorComponent.generated.h"


class UPLLRopeSetting;
class APLLCharacter;
class USphereComponent;
class UPLLHitResult;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTLIFELINE_API UPLLCharacterSensorComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPLLCharacterSensorComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sensor Settings")
	bool bCanSense = false;

	/** *Trace Channel to be used by Traces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
	TEnumAsByte<ETraceTypeQuery> MantlingTraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
	TEnumAsByte<ETraceTypeQuery> WallTraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
	TEnumAsByte<ETraceTypeQuery> RopeTraceChannel;

	/** *Actors to be ignored by Line Trace */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings")
	TArray<AActor*> ActorsToIgnore;

	// Wall Trace Begin Height, beginning from current Location i.e. (ActorLoc.Z + CapsuleHalfHeight)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings|Wall Trace")
	float WallBeginOffset = 0;

	// Wall Trace length, beginning from current Location, i.e. (ActorRot.Forward + CapsuleHalfHeight)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings|Wall Trace")
	float WallTraceForwardOffset = 300;

	// Ledge Trace begin height, beginning from current Location i.e. (ActorLoc.Z + CapsuleHalfHeight)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings|Ledge Trace")
	float LedgeBeginOffset = 150;

	//Ledge Trace forward offset, beginning form current location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings|Ledge Trace")
	float LedgeForwardOffset = 100;

	// Ledge Trace downwards distance, beginning from LedgeBeginOffset + LedgeForwardOffset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings|Ledge Trace")
	float LedgeTraceDownDistance = 160;

	// Ledge Trace begin height, beginning from current Location i.e. (ActorLoc.Z + CapsuleHalfHeight)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings|Ledge Trace")
	float LowLedgeBeginOffset = 150;

	//Ledge Trace forward offset, beginning form current location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings|Ledge Trace")
	float LowLedgeForwardOffset = 100;

	// Ledge Trace downwards distance, beginning from LedgeBeginOffset + LedgeForwardOffset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings|Ledge Trace")
	float LowLedgeTraceDownDistance = 225;

	// Head Trace distance, beginning from eye location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings|Head Trace")
	float HeadTraceDistance = 160;

	// Floor Trace distance, beginning from feet location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings|Floor Trace")
	float FloorTraceDistance = 160;

	// Left Trace distance, beginning from feet location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings|WallRun Trace")
	float LeftTraceDistance = 80;

	// Right Trace distance, beginning from feet location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Settings|WallRun Trace")
	float RightTraceDistance = 80;

	// Right Trace distance, beginning from feet location

	UPROPERTY(EditDefaultsOnly, Category="Trace Settings|Rope Trace")
	UPLLRopeSetting* Settings;

	UPROPERTY(EditDefaultsOnly, Category="Trace Settings|Rope Trace")
	UPLLAimAssistantSettings* AimAssistantSettings;


	UPROPERTY(EditAnywhere, Category="Sensors")
	UPLLHitResult* WallFeetSensor;

	UPROPERTY(EditAnywhere, Category="Sensors")
	UPLLHitResult* WallHipSensor;

	UPROPERTY(EditAnywhere, Category="Sensors")
	UPLLHitResult* WallEyeSensor;

	UPROPERTY(EditAnywhere, Category="Sensors")
	UPLLHitResult* LedgeSensor;

	UPROPERTY(EditAnywhere, Category = "Sensors")
	UPLLHitResult* LowLedgeSensor;

	UPROPERTY(EditAnywhere, Category="Sensors")
	UPLLHitResult* HeadSensor;

	UPROPERTY(EditAnywhere, Category="Sensors")
	UPLLHitResult* FloorSensor;

	UPROPERTY(EditAnywhere, Category="Sensors")
	UPLLHitResult* LeftSensor;

	UPROPERTY(EditAnywhere, Category="Sensors")
	UPLLHitResult* RightSensor;

	UPROPERTY(EditAnywhere, Category="Sensors")
	UPLLHitResult* RopeLineTraceSensor;

	UPROPERTY(EditAnywhere, Category="Sensors")
	UPLLHitResult* BubbleTraceSensor;

	UPROPERTY(EditAnywhere, Category="Sensors")
	UPLLHitResult* GravityTraceSensor;


	//Depricated Use Hit DataAssets instead
	FHitResult WallFeetTraceHit;
	//Depricated Use Hit DataAssets instead
	FHitResult WallHipTraceHit;
	//Depricated Use Hit DataAssets instead
	FHitResult WallEyeTraceHit;
	//Depricated Use Hit DataAssets instead
	FHitResult LedgeTraceHit;
	//Depricated Use Hit DataAssets instead
	FHitResult HeadTraceHit;
	//Depricated Use Hit DataAssets instead
	FHitResult FloorTraceHit;
	//Depricated Use Hit DataAssets instead
	FHitResult LeftTraceHit;
	//Depricated Use Hit DataAssets instead
	FHitResult RightTraceHit;
	//Depricated Use Hit DataAssets instead
	FHitResult RopeTraceHit;

	//Depricated Use Hit DataAssets instead
	bool bDidWallFeetTraceHit;
	//Depricated Use Hit DataAssets instead
	bool bDidWallHipTraceHit;
	//Depricated Use Hit DataAssets instead
	bool bDidWallEyeTraceHit;
	//Depricated Use Hit DataAssets instead
	bool bDidLedgeTraceHit;
	//Depricated Use Hit DataAssets instead
	bool bDidHeadTraceHit;
	//Depricated Use Hit DataAssets instead
	bool bDidFloorTraceHit;
	//Depricated Use Hit DataAssets instead
	bool bDidLeftTraceHit;
	//Depricated Use Hit DataAssets instead
	bool bDidRightTraceHit;
	//Depricated Use Hit DataAssets instead
	bool bDidRopeTraceHit;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool SphereCast(FHitResult& Hit, FVector Start, FVector End, ETraceTypeQuery TraceChannel, const float Radius) const;
	bool RayCast(FHitResult& Hit, FVector Start, FVector End, ETraceTypeQuery TraceChannel) const;

	bool WallFeetTracer();
	bool WallHipTracer();
	bool WallEyeTracer();
	bool LedgeTracer();
	bool LowLedgeTracer();
	bool HeadTracer();
	bool FloorTracer();
	bool LeftTracer();
	bool RightTracer();
	bool RopeTracer();
	bool RopeBubbleTracer();
	bool RopeGravityTracer();
	

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	APLLCharacter* KaiCharacter;
	float          CapsuleHh;
};
