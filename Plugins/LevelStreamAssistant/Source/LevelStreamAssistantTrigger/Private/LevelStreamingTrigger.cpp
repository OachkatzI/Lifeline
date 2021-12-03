// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "LevelStreamingTrigger.h"
#include "GateComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/LevelStreaming.h"

#if WITH_EDITORONLY_DATA
#include "UObject/ConstructorHelpers.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#endif

ALevelStreamingTrigger::ALevelStreamingTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	bIsTwoWay = true;
	bMakeVisibleAfterLoad = true;
	bShouldBlockOnLoad = false;
	bShouldBlockOnUnload = false;

	TriggerComponent = CreateDefaultSubobject<UGateComponent>(TEXT("Trigger"));
	SetRootComponent(TriggerComponent);
	TriggerComponent->ShapeColor = FColor::Silver;
	TriggerComponent->SetWorldScale3D(FVector(2.f, 4.f, 4.f));

#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("EntryDirection"));
	ArrowComponent->SetupAttachment(TriggerComponent);
	ArrowComponent->SetUsingAbsoluteScale(true);
	ArrowComponent->SetIsVisualizationComponent(true);
	ArrowComponent->bIsScreenSizeScaled = true;
	ArrowComponent->bUseAttachParentBound = true;
	//ArrowComponent->bVisualizeComponent = true;

	ConstructorHelpers::FObjectFinderOptional<UTexture2D> TriggerTextureObject(TEXT("/LevelStreamAssistant/TriggerBillboard.TriggerBillboard"));
	BillboardComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	BillboardComponent->SetupAttachment(TriggerComponent);
	BillboardComponent->SetUsingAbsoluteScale(true);
	BillboardComponent->SetIsVisualizationComponent(true);
	BillboardComponent->SetRelativeScale3D(FVector::OneVector);
	BillboardComponent->bIsScreenSizeScaled = true;
	BillboardComponent->bUseInEditorScaling = true;
	BillboardComponent->bUseAttachParentBound = true;
	BillboardComponent->Sprite = TriggerTextureObject.Get();
#endif
}

void ALevelStreamingTrigger::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITORONLY_DATA
	if (ArrowComponent) ArrowComponent->SetVisibility(bIsTwoWay);
#endif
}

void ALevelStreamingTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(TriggerComponent))
	{
		if (bIsTwoWay)
		{
			TriggerComponent->OnActorEntered.AddDynamic(this, &ALevelStreamingTrigger::HandleActorEntered);
			TriggerComponent->OnActorExited.AddDynamic(this, &ALevelStreamingTrigger::HandleActorExited);
		}
		else
		{
			TriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ALevelStreamingTrigger::HandleBeginOverlap);
		}
	}
}

void ALevelStreamingTrigger::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (ensure(TriggerComponent))
	{
		TriggerComponent->OnActorEntered.RemoveAll(this);
		TriggerComponent->OnActorExited.RemoveAll(this);
		TriggerComponent->OnComponentBeginOverlap.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ALevelStreamingTrigger::HandleActorEntered(AActor* EnteredActor, UPrimitiveComponent* EnteredComponent)
{
	// start working if entered actor is a controlled pawn.
	if (APawn* OverlappedPawn = Cast<APawn>(EnteredActor))
	{
		if (OverlappedPawn->IsPawnControlled())
		{
			SetLevelsToStream(LevelsToLoad, LevelsToUnload);
		}
	}
}

void ALevelStreamingTrigger::HandleActorExited(AActor* ExitedActor, UPrimitiveComponent* ExitedComponent)
{
	// start working if exited actor is a controlled pawn.
	if (APawn* OverlappedPawn = Cast<APawn>(ExitedActor))
	{
		if (OverlappedPawn->IsPawnControlled())
		{
			SetLevelsToStream(LevelsToUnload, LevelsToLoad);
		}
	}
}

void ALevelStreamingTrigger::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HandleActorEntered(OtherActor, OtherComp);
}

void ALevelStreamingTrigger::SetLevelsToStream(const TArray<TSoftObjectPtr<UWorld>>& InLevelsToLoad, const TArray<TSoftObjectPtr<UWorld>>& InLevelsToUnload)
{
	// unload loaded levels
	for (TSoftObjectPtr<UWorld> Level : InLevelsToUnload)
	{
		SetLevelToStream(*Level.GetLongPackageName(), /*bShouldBeLoaded*/false, /*bShouldBeVisible*/false, bShouldBlockOnUnload);
	}

	// load unloaded levels
	for (TSoftObjectPtr<UWorld> Level : InLevelsToLoad)
	{
		SetLevelToStream(*Level.GetLongPackageName(), /*bShouldBeLoaded*/true, bMakeVisibleAfterLoad, bShouldBlockOnLoad);
	}
}

void ALevelStreamingTrigger::SetLevelToStream(const FName& LevelName, bool bShouldBeLoaded, bool bShouldBeVisible, bool bShouldBlock)
{
	if (UWorld* World = GetWorld())
	{
		if (ULevelStreaming* LevelStreaming = FStreamLevelAction::FindAndCacheLevelStreamingObject(LevelName, World))
		{
			if (LevelStreaming)
			{
				LevelStreaming->SetShouldBeLoaded(bShouldBeLoaded);
				LevelStreaming->SetShouldBeVisible(bShouldBeVisible);

				if (bShouldBeLoaded) LevelStreaming->bShouldBlockOnLoad = bShouldBlock;
				else LevelStreaming->bShouldBlockOnUnload = bShouldBlock;
			}
		}
	}
}
