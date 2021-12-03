// Project LifeLine

#include "Actors/ItemShowcase.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"


AItemShowcase::AItemShowcase()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentShowcaseItem = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CurrentShowcaseItem"));
	CurrentShowcaseItem->SetupAttachment(RootComponent);

	CurrentShowcaseItemIndex = 0;
	bShuffle = true;
	ShowcaseTimePerItem = 4.f;
	ItemTransitionTime = .5f;
	ItemRotationSpeed = 150.f;
	AxisRotationWeight = { 0.f, 1.f, 0.f };
	CurrentShowcasingMode = FadingIn;
}


void AItemShowcase::BeginPlay()
{
	Super::BeginPlay();

	check(ItemFadeInOutCurve);
	check(ItemShowcaseList.Num() > 0);
	
	CurrentShowcaseItem->SetStaticMesh(ItemShowcaseList[CurrentShowcaseItemIndex]);
	CurrentShowcaseItem->SetCollisionProfileName(TEXT("NoCollision"));

	ItemFadeInOutCurve->GetTimeRange(CurveMin, CurveMax);
	
	StartItemFadeInOut();
}


void AItemShowcase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentShowcaseItem)
	{
		CurrentShowcaseItem->AddLocalRotation(AxisRotationWeight * ItemRotationSpeed * DeltaTime);
		CurrentShowcaseItem->SetWorldScale3D(GetDesiredItemScale());
	}
}


void AItemShowcase::ShowcaseNextItem()
{
	CurrentShowcaseItemIndex = GetNextItemIndex();
	CurrentShowcaseItem->SetStaticMesh(ItemShowcaseList[CurrentShowcaseItemIndex]);

	OnNextItemShowcased.Broadcast();
}


uint8 AItemShowcase::GetNextItemIndex() const
{
	if (!bShuffle)
	{
		return (CurrentShowcaseItemIndex + 1) % ItemShowcaseList.Num();
	}

	uint8 NewItemIndex = CurrentShowcaseItemIndex;
	while (NewItemIndex == CurrentShowcaseItemIndex)
	{
		NewItemIndex = FMath::RandRange(0, ItemShowcaseList.Num() - 1);
	}
	
	return NewItemIndex;
}


void AItemShowcase::StartItemFadeInOut()
{
	switch(CurrentShowcasingMode)
	{
	case FadingIn:
		GetWorld()->GetTimerManager().SetTimer(ShowcaseTimer, this, &AItemShowcase::EndFadingIn, ItemTransitionTime);
		break;

	case Showcasing:
		GetWorld()->GetTimerManager().SetTimer(ShowcaseTimer, this, &AItemShowcase::StartItemFadeInOut, ItemTransitionTime);
		CurrentShowcasingMode = FadingOut;
		break;
		
	case FadingOut:
		CurrentShowcasingMode = FadingIn;
		ShowcaseNextItem();
		GetWorld()->GetTimerManager().SetTimer(ShowcaseTimer, this, &AItemShowcase::EndFadingIn, ItemTransitionTime);
		break;
		
	default:
		break;
	}
}


void AItemShowcase::EndFadingIn()
{
	CurrentShowcasingMode = Showcasing;
	GetWorld()->GetTimerManager().SetTimer(ShowcaseTimer, this, &AItemShowcase::StartItemFadeInOut, ShowcaseTimePerItem);
}


FVector AItemShowcase::GetDesiredItemScale() const
{
	if (CurrentShowcasingMode == Showcasing)
	{
		return { 1.f, 1.f, 1.f };
	}

	const float ElapsedTimerSeconds = GetWorld()->GetTimerManager().GetTimerElapsed(ShowcaseTimer);
	const float CurveTime = UKismetMathLibrary::MapRangeClamped(ElapsedTimerSeconds, 0.f, ItemTransitionTime, CurveMin + 0.01, CurveMax);
	float NewItemScale = ItemFadeInOutCurve->GetFloatValue(CurveTime);

	if (CurrentShowcasingMode == FadingIn)
	{
		NewItemScale = 1 - NewItemScale;
	}
	
	return { NewItemScale, NewItemScale, NewItemScale };
}
