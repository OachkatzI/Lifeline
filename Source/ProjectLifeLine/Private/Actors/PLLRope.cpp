// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/PLLRope.h"
#include <string>
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CableComponent.h"
#include "Characters/PLLCharacter.h"
#include "GameFramework/Character.h"
#include "SceneComponent/PLLRopeShooterComponent.h"
#include "UDataAsset/PLLRopeSetting.h"


static bool             DebugShowRopeInfo = true;
FAutoConsoleVariableRef CVARDebugRopeInfoPrinting(TEXT("PLL.RopeInfo"), DebugShowRopeInfo, TEXT("Print rope info to screen"), ECVF_Cheat);


APLLRope::APLLRope()
{
	PrimaryActorTick.bCanEverTick = true;

	bIsAttached = false;

	Cable = CreateDefaultSubobject<UCableComponent>(TEXT("CableComp"));
}


void APLLRope::BeginPlay()
{
	Super::BeginPlay();

	RopeAnchor = GetWorld()->SpawnActor<APLLRopeAnchor>(AnchorBluePrint, GetActorLocation(), GetActorRotation(), FActorSpawnParameters());
	RopeAnchor->SetRope(this);
	RopeAnchor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

	Cable->SetAttachEndTo(RopeAnchor, "None");
	Cable->SetVisibility(false);

	KaiCharacter = Cast<APLLCharacter>(GetOwner());
	check(KaiCharacter);
}


void APLLRope::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsAttached)
	{
		CurrentSwingAngle = CalculateReleaseAngle();

		if (FMath::IsNearlyEqual(CurrentSwingAngle, Settings->PerfectReleaseAngle, Settings->PerfectReleaseAngleTolerance))
		{
			// Map how close you are to perfect release angle (0 - 1 range)
			CurrentSwingAnglePerfectionScale = 0.f + ((1.f - 0.f) / ((Settings->PerfectReleaseAngle + Settings->PerfectReleaseAngleTolerance
			) - (Settings->PerfectReleaseAngle - Settings->PerfectReleaseAngleTolerance))) * (CurrentSwingAngle - (Settings->
				PerfectReleaseAngle - Settings->PerfectReleaseAngleTolerance));
		}
		else
		{
			CurrentSwingAnglePerfectionScale = 0.f;
		}

		if (DebugShowRopeInfo)
		{
			UKismetSystemLibrary::PrintString(this, FString("[Current RopeSwing Angle] ").Append(std::to_string(CurrentSwingAngle).c_str()),
			                                  true, false, FLinearColor::Red, 0.f);
			UKismetSystemLibrary::PrintString(this, FString("[Perfect Angle Scale] ").
			                                  Append(std::to_string(CurrentSwingAnglePerfectionScale).c_str()), true, false,
			                                  FLinearColor::Red, 0.f);
		}
	}
}


void APLLRope::Attach(const bool bUseMainAnchor, const FRopeAttachmentParams Params)
{
	Cable->SetSimulatePhysics(false);
	Cable->ToggleVisibility();

	RopeAnchor->Attach(Params);

	Cable->SetSimulatePhysics(true);
	RopeLength         = Params.NewRopeLength;
	Cable->CableLength = Params.NewRopeLength / 1.5f;

	bIsAttached = true;

	OnRopeAttached.Broadcast();
}


void APLLRope::Release()
{
	if (!bIsAttached)
	{
		return;
	}

	RopeAnchor->Release();

	Cable->SetSimulatePhysics(false);
	Cable->CableLength = 20.f;
	Cable->ToggleVisibility();

	bIsAttached = false;

	OnRopeReleased.Broadcast(CurrentSwingAngle);
}


float APLLRope::CalculateReleaseAngle()
{
	// Bad way to get character, but works for now, since we only have 1 Player
	if (UGameplayStatics::GetPlayerCharacter(this, 0)->GetVelocity().Z < 0.f)
	{
		// We don't want a boost when swinging downwards
		return 0.f;
	}
	
	FVector    CharacterVector = GetOwner()->GetActorLocation() - GetCurrentSwingPoint();
	CharacterVector.Normalize();

	FVector RopeAnchorVector = GetOwner()->GetActorLocation();
	RopeAnchorVector.Z       = GetCurrentSwingPoint().Z;
	RopeAnchorVector -= GetCurrentSwingPoint();
	RopeAnchorVector.Normalize();

	return FMath::RadiansToDegrees(acosf(FVector::DotProduct(CharacterVector, RopeAnchorVector)));
}
