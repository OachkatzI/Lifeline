// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "GateComponent.h"

UGateComponent::UGateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGateComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddUniqueDynamic(this, &UGateComponent::HandleBeginOverlap);
	OnComponentEndOverlap.AddUniqueDynamic(this, &UGateComponent::HandleEndOverlap);
}

void UGateComponent::EndPlay(EEndPlayReason::Type Reason)
{
	OnComponentBeginOverlap.RemoveAll(this);
	OnComponentEndOverlap.RemoveAll(this);

	Super::EndPlay(Reason);
}

void UGateComponent::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		FVector GateDirection = GetForwardVector();
		FVector EnteredDirection = OtherActor->GetActorLocation() - GetComponentLocation();

		float EnteredCosineYaw = GateDirection.CosineAngle2D(EnteredDirection);
		float EnteredYaw = FMath::Acos(EnteredCosineYaw);

		// if actor entered from behind.
		if (EnteredYaw > FMath::DegreesToRadians(90.f))
		{
			OnActorEntered.Broadcast(OtherActor, OtherComp);
		}
	}
}

void UGateComponent::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		FVector GateDirection = GetForwardVector();
		FVector EnteredDirection = OtherActor->GetActorLocation() - GetComponentLocation();

		float EnteredCosineYaw = GateDirection.CosineAngle2D(EnteredDirection);
		float EnteredYaw = FMath::Acos(EnteredCosineYaw);

		// if actor exited from behind.
		if (EnteredYaw > FMath::DegreesToRadians(90.f))
		{
			OnActorExited.Broadcast(OtherActor, OtherComp);
		}
	}
}
