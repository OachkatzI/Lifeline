// Project LifeLine


#include "ActorComponent/PLLResponsiveHUDComponent.h"


#include "Characters/PLLCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "SceneComponent/PLLCharacterSensorComponent.h"

// Sets default values for this component's properties
UPLLResponsiveHUDComponent::UPLLResponsiveHUDComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	MyHUD = Cast<APLLHUD>(GetOwner());
}

// Called every frame
void UPLLResponsiveHUDComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!Sensor)
	{
		UE_LOG(LogTemp, Warning, TEXT("ResponsiveHudComp: Sensor not found") );
		return;
	}
	
	if(Sensor->bDidRopeTraceHit)
	{
		MyHUD->SetRopeAttachable(true);
	}
	else
	{
		MyHUD->SetRopeAttachable(false);
	}
}

