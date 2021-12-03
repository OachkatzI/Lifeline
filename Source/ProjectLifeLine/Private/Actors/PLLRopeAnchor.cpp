// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PLLRopeAnchor.h"
#include "Actors/PLLRope.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"


APLLRopeAnchor::APLLRopeAnchor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MeshComp->SetVisibility(false);
}


void APLLRopeAnchor::BeginPlay()
{
	Super::BeginPlay();
}


void APLLRopeAnchor::Attach(FRopeAttachmentParams Params)
{
	SetActorLocation(Params.PositionToAttachTo);
	AttachToActor(Params.ActorToAttachTo, FAttachmentTransformRules::KeepWorldTransform);	
}


void APLLRopeAnchor::Release()
{
	AttachToActor(Rope, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}
