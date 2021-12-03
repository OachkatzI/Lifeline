// Project LifeLine

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UI/PLLUserWidget.h"

#include "PLLGameModeMenu.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIndicesReady);

UCLASS()
class PROJECTLIFELINE_API APLLGameModeMenu : public AGameModeBase
{
	GENERATED_BODY()

	APLLGameModeMenu(const FObjectInitializer& ObjectInitializer);
	
public:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
protected:

	UPROPERTY()
	UPLLUserWidget* MainMenuWidget;

	UPROPERTY()
	TArray<AActor*> mainMenuPlayerControllers;
	
	TSubclassOf<UPLLUserWidget> mainMenuWidgetClass;
	
	void CreateMainMenu();
	
public:
	
	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnIndicesReady IndicesReady;
	
};
