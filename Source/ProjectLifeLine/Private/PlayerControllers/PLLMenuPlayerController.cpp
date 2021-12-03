// Project LifeLine

#include "PlayerControllers/PLLMenuPlayerController.h"
#include <string>
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Fill out your copyright notice in the Description page of Project Settings.

void APLLMenuPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("ApplyLobbySelection", IE_Pressed, this, &APLLMenuPlayerController::FSelectionApplied);
	InputComponent->BindAction("MenuBack", IE_Pressed, this, &APLLMenuPlayerController::PopFromMenuStack);
	InputComponent->BindAction("MenuNavigationDown", IE_Pressed, this, &APLLMenuPlayerController::FMainMenuNavigated);
	InputComponent->BindAction("MenuNavigationUp", IE_Pressed, this, &APLLMenuPlayerController::FMainMenuNavigated);
	InputComponent->BindAction("MenuNavigationRight", IE_Pressed, this, &APLLMenuPlayerController::FMainMenuNavigated);
	InputComponent->BindAction("MenuNavigationLeft", IE_Pressed, this, &APLLMenuPlayerController::FMainMenuNavigated);
}

void APLLMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	mainMenuGameMode = Cast<APLLGameModeMenu>(GetWorld()->GetAuthGameMode());
}

void APLLMenuPlayerController::FSelectionApplied()
{
	SelectionApplied.Broadcast();
}

void APLLMenuPlayerController::FMainMenuNavigated()
{
	MainMenuNavigated.Broadcast();
}

void APLLMenuPlayerController::PushToMenuStack(UPLLUserWidget* newWidget)
{
	if (widgetStack.Num() != 0)
	{
		widgetStack.Last()->RemoveFromParent();
	}

	newWidget->AddToViewport();
	widgetStack.Add(newWidget);
	
	UKismetSystemLibrary::PrintString(this, FString(std::to_string(widgetStack.Num()).c_str()));
}

void APLLMenuPlayerController::PopFromMenuStack()
{
	if (widgetStack.Num() <= 1)
	{
		return;
	}

	widgetStack.Pop()->RemoveFromParent();
	UKismetSystemLibrary::PrintString(this, FString(std::to_string(widgetStack.Num()).c_str()));
	widgetStack.Last()->AddToViewport();
}

bool APLLMenuPlayerController::GetIsAssigned() const
{
	return bIsAssigned;
}

void APLLMenuPlayerController::SetIsAssigned(bool assigned)
{
	bIsAssigned = assigned;
}
