// Project LifeLine


#include "Gamemodes/PLLGameModeMenu.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerControllers/PLLMenuPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UI/PLLUserWidget.h"
#include "Gamemodes/PLLGameModePlay.h"

APLLGameModeMenu::APLLGameModeMenu(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultPawnClass = nullptr;
	PrimaryActorTick.bCanEverTick = true;
	PlayerControllerClass = APLLMenuPlayerController::StaticClass();

	static ConstructorHelpers::FClassFinder<UPLLUserWidget> mainMenuWidgetClassFinder(TEXT("/Game/UI/MainMenu/BP_PLLMainMenuWidget"));
	mainMenuWidgetClass = mainMenuWidgetClassFinder.Class;
}

void APLLGameModeMenu::BeginPlay()
{
	Super::BeginPlay();
	
	CreateMainMenu();
}

void APLLGameModeMenu::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void APLLGameModeMenu::CreateMainMenu()
{
	auto mainMenuControllerPlayer1 = Cast<APLLMenuPlayerController>(UGameplayStatics::GetPlayerControllerFromID(this, 0));
	
	// Main Menu
	MainMenuWidget = Cast<UPLLUserWidget>(CreateWidget(mainMenuControllerPlayer1, mainMenuWidgetClass));

	mainMenuControllerPlayer1->PushToMenuStack(MainMenuWidget);
	FInputModeGameAndUI inputMode;
	inputMode.SetWidgetToFocus(MainMenuWidget->GetSlateWidgetFromName(FName(TEXT("Button_Start"))));
	mainMenuControllerPlayer1->SetInputMode(inputMode);
	mainMenuControllerPlayer1->bShowMouseCursor = false;
}

