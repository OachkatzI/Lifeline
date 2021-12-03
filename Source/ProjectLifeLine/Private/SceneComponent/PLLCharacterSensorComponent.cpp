// Project LifeLine


#include "SceneComponent/PLLCharacterSensorComponent.h"

#include "Camera/CameraComponent.h"
#include "Characters/PLLCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "PlayerControllers/PLLPlayerController.h"
#include "UDataAsset/PLLRopeSetting.h"
#include "UDataAsset/PLLHitResult.h"


static bool             DebugShowSensorOutlines = false;
FAutoConsoleVariableRef CVARDebugShowSensorOutlinesPrinting(TEXT("PLL.SensorOutlines"), DebugShowSensorOutlines,
                                                            TEXT("Shows the active Sensor Outlines"), ECVF_Cheat);

// Sets default values for this component's properties
UPLLCharacterSensorComponent::UPLLCharacterSensorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UPLLCharacterSensorComponent::BeginPlay()
{
	Super::BeginPlay();
	KaiCharacter = Cast<APLLCharacter>(GetOwner());
	CapsuleHh    = KaiCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}


// Called every frame
void UPLLCharacterSensorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bCanSense) return;
	WallFeetSensor->bDidHit      = WallFeetTracer();
	WallHipSensor->bDidHit       = WallHipTracer();
	WallEyeSensor->bDidHit       = WallEyeTracer();
	LedgeSensor->bDidHit         = LedgeTracer();
	LowLedgeSensor->bDidHit      = LowLedgeTracer();
	HeadSensor->bDidHit          = HeadTracer();
	FloorSensor->bDidHit         = FloorTracer();
	LeftSensor->bDidHit          = LeftTracer();
	RightSensor->bDidHit         = RightTracer();
	RopeLineTraceSensor->bDidHit = RopeTracer();
	BubbleTraceSensor->bDidHit   = RopeBubbleTracer();
	GravityTraceSensor->bDidHit  = RopeGravityTracer();

	//Deprecated will be removed later on
	bDidWallFeetTraceHit = WallFeetSensor->bDidHit;
	bDidWallHipTraceHit  = WallHipSensor->bDidHit;
	bDidWallEyeTraceHit  = WallEyeSensor->bDidHit;
	bDidLedgeTraceHit    = LedgeSensor->bDidHit;
	bDidHeadTraceHit     = HeadSensor->bDidHit;
	bDidFloorTraceHit    = FloorSensor->bDidHit;
	bDidLeftTraceHit     = LeftSensor->bDidHit;
	bDidRightTraceHit    = RightSensor->bDidHit;
	bDidRopeTraceHit     = RopeLineTraceSensor->bDidHit;

	//Deprecated will be removed later on
	WallFeetTraceHit = WallFeetSensor->Hit;
	WallHipTraceHit  = WallHipSensor->Hit;
	WallEyeTraceHit  = WallEyeSensor->Hit;
	LedgeTraceHit    = LedgeSensor->Hit;
	HeadTraceHit     = HeadSensor->Hit;
	FloorTraceHit    = FloorSensor->Hit;
	LeftTraceHit     = LeftSensor->Hit;
	RightTraceHit    = RightSensor->Hit;
	RopeTraceHit     = RopeLineTraceSensor->Hit;
}


bool UPLLCharacterSensorComponent::SphereCast(FHitResult& Hit, FVector Start, FVector End, ETraceTypeQuery TraceChannel,
                                              const float Radius = 20) const
{
	return UKismetSystemLibrary::SphereTraceSingle(GetWorld(),
	                                               Start,
	                                               End,
	                                               Radius,
	                                               TraceChannel,
	                                               false,
	                                               ActorsToIgnore,
	                                               DebugShowSensorOutlines ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
	                                               Hit,
	                                               true);
}


bool UPLLCharacterSensorComponent::RayCast(FHitResult& Hit, FVector Start, FVector End, ETraceTypeQuery TraceChannel) const
{
	return UKismetSystemLibrary::LineTraceSingle(
	                                             GetWorld(),
	                                             Start,
	                                             End,
	                                             TraceChannel,
	                                             false,
	                                             ActorsToIgnore,
	                                             DebugShowSensorOutlines ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
	                                             Hit,
	                                             true);
}


bool UPLLCharacterSensorComponent::WallFeetTracer()
{
	FVector Start = KaiCharacter->GetActorLocation();
	Start.Z -= CapsuleHh * 0.75;
	FVector End = UKismetMathLibrary::GetForwardVector(GetOwner()->GetActorRotation());
	End *= FVector(WallTraceForwardOffset, WallTraceForwardOffset, 1);
	End = Start + End;

	return SphereCast(WallFeetSensor->Hit, Start, End, MantlingTraceChannel);
}


bool UPLLCharacterSensorComponent::WallHipTracer()
{
	FVector Start = KaiCharacter->GetActorLocation();
	Start.Z += WallBeginOffset;
	FVector End = UKismetMathLibrary::GetForwardVector(GetOwner()->GetActorRotation());
	End *= FVector(WallTraceForwardOffset, WallTraceForwardOffset, 1);
	End = Start + End;

	return SphereCast(WallHipSensor->Hit, Start, End, MantlingTraceChannel);
}


bool UPLLCharacterSensorComponent::WallEyeTracer()
{
	FVector Start = KaiCharacter->GetActorLocation();
	Start.Z += CapsuleHh * 0.75;
	FVector End = UKismetMathLibrary::GetForwardVector(GetOwner()->GetActorRotation());
	End *= FVector(WallTraceForwardOffset, WallTraceForwardOffset, 1);
	End = Start + End;

	return SphereCast(WallEyeSensor->Hit, Start, End, MantlingTraceChannel);
}


bool UPLLCharacterSensorComponent::LedgeTracer()
{
	FVector Start = KaiCharacter->GetActorLocation();
	Start.Z += LedgeBeginOffset;

	FVector Forward = KaiCharacter->GetActorRotation().Vector();
	Forward *= LedgeForwardOffset;

	Start += Forward;

	FVector End = Start;
	End.Z -= LedgeTraceDownDistance;

	return SphereCast(LedgeSensor->Hit, Start, End, MantlingTraceChannel);
}


bool UPLLCharacterSensorComponent::LowLedgeTracer()
{
	FVector Start = KaiCharacter->GetActorLocation();
	Start.Z += LowLedgeBeginOffset;

	FVector Forward = KaiCharacter->GetActorRotation().Vector();
	Forward *= LowLedgeForwardOffset;

	Start += Forward;

	FVector End = Start;
	End.Z -= LowLedgeTraceDownDistance;

	return SphereCast(LowLedgeSensor->Hit, Start, End, MantlingTraceChannel);
}


bool UPLLCharacterSensorComponent::HeadTracer()
{
	FVector  EyeLocation;
	FRotator EyeRotation;
	KaiCharacter->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	const FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * HeadTraceDistance);

	return RayCast(HeadSensor->Hit, EyeLocation, TraceEnd, MantlingTraceChannel);
	//
	// FVector  EyeLocation = KaiCharacter->GetFirstPersonCamera()->GetComponentLocation();
	// FRotator EyeRotation = KaiCharacter->GetFirstPersonCamera()->GetForwardVector().Rotation();
	//
	// const FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * HeadTraceDistance);
	// return RayCast(HeadSensor->Hit, EyeLocation, TraceEnd, MantlingTraceChannel);
}


bool UPLLCharacterSensorComponent::FloorTracer()
{
	const FVector Start = KaiCharacter->GetActorLocation() + FVector(0, 0, -CapsuleHh);
	const FVector End   = Start + FVector(0, 0, -FloorTraceDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.bTraceComplex = true;

	return GetWorld()->LineTraceSingleByChannel(FloorSensor->Hit, Start, End, ECC_Visibility, QueryParams);
}


bool UPLLCharacterSensorComponent::LeftTracer()
{
	const FVector Start = KaiCharacter->GetActorLocation() + FVector(0, 0, -CapsuleHh * 0.8);
	const FVector End   = Start + -KaiCharacter->GetActorRightVector() * LeftTraceDistance;

	return RayCast(LeftSensor->Hit, Start, End, MantlingTraceChannel);
}


bool UPLLCharacterSensorComponent::RightTracer()
{
	const FVector Start = KaiCharacter->GetActorLocation() + FVector(0, 0, -CapsuleHh * 0.8);;
	const FVector End   = Start + KaiCharacter->GetActorRightVector() * LeftTraceDistance;

	return RayCast(RightSensor->Hit, Start, End, MantlingTraceChannel);
}


bool UPLLCharacterSensorComponent::RopeTracer()
{
	FVector  EyeLocation;
	FRotator EyeRotation;

	KaiCharacter->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	const FVector TraceEnd = EyeLocation + (UKismetMathLibrary::GetForwardVector(EyeRotation) * Settings->RopeMaxLength);

	return RayCast(RopeLineTraceSensor->Hit, EyeLocation, TraceEnd, RopeTraceChannel);
}


bool UPLLCharacterSensorComponent::RopeBubbleTracer()
{
	FVector  EyeLocation;
	FRotator EyeRotation;

	KaiCharacter->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	const FVector TraceEnd = EyeLocation + (UKismetMathLibrary::GetForwardVector(EyeRotation) * Settings->RopeMaxLength);

	return SphereCast(BubbleTraceSensor->Hit, EyeLocation, TraceEnd, RopeTraceChannel, AimAssistantSettings->BubbleCursorRadius);
}


bool UPLLCharacterSensorComponent::RopeGravityTracer()
{
	FVector  EyeLocation;
	FRotator EyeRotation;

	KaiCharacter->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	const FVector TraceEnd = EyeLocation + (UKismetMathLibrary::GetForwardVector(EyeRotation) * Settings->RopeMaxLength);

	return SphereCast(GravityTraceSensor->Hit, EyeLocation, TraceEnd, RopeTraceChannel, AimAssistantSettings->GravityRadius);
}
