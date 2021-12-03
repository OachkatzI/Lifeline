// Project LifeLine

#pragma once

#include "CoreMinimal.h"
#include "Characters/PLLCharacter.h"
#include "Gamemodes/PLLGameModeMenu.h"
#include "PLLMenuPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPreviousCharacterSelected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNextCharacterSelected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPreviousTeamSelected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNextTeamSelected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectionApplied);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyJoined);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMainMenuNavigated);

UCLASS()
class PROJECTLIFELINE_API APLLMenuPlayerController : public APlayerController
{
// Fill out your copyright notice in the Description page of Project Settings.
	GENERATED_BODY()

	void SetupInputComponent() override;

public:

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnMainMenuNavigated MainMenuNavigated;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnMainMenuNavigated SelectionApplied;
	
	UFUNCTION(BlueprintCallable)
	void PushToMenuStack(UPLLUserWidget* newWidget);

	UFUNCTION(BlueprintCallable)
	void PopFromMenuStack();

	UFUNCTION(BlueprintCallable)
	bool GetIsAssigned() const;

	UFUNCTION(BlueprintCallable)
	void SetIsAssigned(bool assigned);
	
protected:

	UPROPERTY()
	bool bIsAssigned;

	UPROPERTY()
	TArray<UUserWidget*> widgetStack;

	UPROPERTY()
	APLLGameModeMenu* mainMenuGameMode;
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void FSelectionApplied();

	UFUNCTION(BlueprintCallable)
	void FMainMenuNavigated();
	
};

