// Project LifeLine


#include "Actors/PLLCheckpoint.h"

#include "DrawDebugHelpers.h"
#include "Characters/Movement/PLLCharacterMovementComponent.h"
#include "Gamemodes/PLLGameModePlay.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

static bool             DebugShowCheckpointOutlines = true;
FAutoConsoleVariableRef CVARDebugShowCheckpointOutlinesPrinting(TEXT("PLL.CheckpointOutlines"), DebugShowCheckpointOutlines,
                                                                TEXT("Shows the active Checkpoint Outlines"), ECVF_Cheat);


APLLCheckpoint::APLLCheckpoint()
{
	//Register Events
	OnActorBeginOverlap.AddDynamic(this, &APLLCheckpoint::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &APLLCheckpoint::OnOverlapEnd);
}


void APLLCheckpoint::Tick(float DeltaSeconds)
{
	//Draw Checkpoint Debug outlines 
	DebugShowCheckpointOutlines ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
}


void APLLCheckpoint::BeginPlay()
{
	Super::BeginPlay();
	DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Purple, true, -1, 0, 5);

	//Get Game Mode
	MyGameMode = Cast< APLLGameModePlay>(UGameplayStatics::GetGameMode(GetWorld()));
}


void APLLCheckpoint::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor->Tags.Contains("Player"))
	{
		if (this->FadeOnEnter || this->DisableInputOnEnter)
		{
			this->OnCheckpointEntered();
		}

		//safe player location
		GEngine->AddOnScreenDebugMessage(24, 10, FColor::Red,TEXT("Last Safed PlayerLocation: " + this->GetActorLocation().ToString()));

		// TODO: build error
		//set Last Checkpoint of GameMode
		if(MyGameMode->IsValidLowLevel())
		{
			MyGameMode->SetLastCheckpoint(this);
		}
		
		//call blueprint function
		this->CheckpointAction();
	}
}


void APLLCheckpoint::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
}


void APLLCheckpoint::Teleport(APLLCharacter* TargetCharacter, APLLCheckpoint* TargetCheckpoint, FRotator TargetRotation)
{
	this->ResetMomentum(TargetCharacter);

	TargetCharacter->TeleportTo(TargetCheckpoint->GetActorLocation(), TargetRotation);
}


void APLLCheckpoint::ResetMomentum(APLLCharacter* TargetCharacter)
{
	TargetCharacter->FindComponentByClass<UPLLCharacterMovementComponent>()->ClearAccumulatedForces();
	TargetCharacter->FindComponentByClass<UPLLCharacterMovementComponent>()->Velocity = FVector(0, 0, 0);
}
