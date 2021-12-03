// Project LifeLine


#include "Gamemodes/PLLGameModePlay.h"

#include "Actors/PLLCheckpoint.h"
#include "Gamemodes/PLLGameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerControllers/PLLPlayerController.h"

APLLGameModePlay::APLLGameModePlay()
{
    //activate tick
    PrimaryActorTick.bStartWithTickEnabled = true;
    PrimaryActorTick.bCanEverTick = true;

    //set default values 
    SaveGameTimer = 0.0f;
    loadingSequenceTimer = 0.f;
    CanSaveGame = true;
    IsPreparingSave = false;
    GameLoaded = false;
    DoesSaveGameExist = true;

}

void APLLGameModePlay::BeginPlay()
{
    KaiCharacter = Cast<APLLCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    MyStartLocation = KaiCharacter->GetActorLocation();

    StartFromEditorLocation = GetWorld()->WorldType == EWorldType::PIE ? StartFromEditorLocation : false;

    if(StartFromEditorLocation)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, FString::Printf(TEXT("START FROM EDITOR LOCATION. NOT Checkpoint")));
    }
    
    SetInputMode();
    SetDefaultStartCheckpoint();

    UPLLGameUserSettings* MyGameUserSettings = UPLLGameUserSettings::GetPLLGameUserSettings();
    if(MyGameUserSettings->SaveSlotNameArray.Num() <= 0)
    {
        CurrentSaveSlotName = MyGameUserSettings->GenerateNewSaveSlotName();
        MyGameUserSettings->AddSaveSlotName(FName(CurrentSaveSlotName));
        MyGameUserSettings->SetSelectedSaveSlotName(FName(CurrentSaveSlotName));
        DoesSaveGameExist = false;
    }
    else
    {
        CurrentSaveSlotName = MyGameUserSettings->GetSelectedSaveSlotName().ToString();
    }

    if(MyLastCheckpoint)
    {
        MyLastCheckpoint = DefaultStartCheckpoint;
    }
}

void APLLGameModePlay::Tick(float DeltaSeconds)
{

    if(DoesSaveGameExist)
    {
        LoadGameLogic(DeltaSeconds);
    }
    
    if(!CanSaveGame)
    {
        SaveGameTimer += DeltaSeconds;

        if(SaveGameTimer >= 3.0f)
        {
            CanSaveGame = true;
            SaveGameTimer = 0;
        }
    }
}

void APLLGameModePlay::CompleteMission()
{
    //TODO: everything that needs to be done before leaving current level

    MyLastCheckpoint = DefaultStartCheckpoint;

    SafeGameLogic();
    
    UGameplayStatics::OpenLevel(GetWorld(), "GameOver");
}

void APLLGameModePlay::ResetToLastCheckpoint()
{
    GEngine->AddOnScreenDebugMessage(23, 10.0f, FColor::Red,TEXT("Load last checkpoint"));

    KaiCharacter->GetPLLController()->OnRespawn();
	
    //reset to last checkpoint or if not yet passed to start point
    if(MyLastCheckpoint)
    {
        KaiCharacter->SetActorLocation(MyLastCheckpoint->GetActorLocation());
        MyLastCheckpoint->OnResetToCheckopoint();
    }
    else
    {
        KaiCharacter->SetActorLocation(MyStartLocation);
    }
    
}

void APLLGameModePlay::SetLastCheckpoint(APLLCheckpoint* LastCheckpoint)
{
    if(!LastCheckpoint || LastCheckpoint == MyLastCheckpoint )
    {
        return;
    }

    MyLastCheckpoint = LastCheckpoint;

    SafeGameLogic();
}


void APLLGameModePlay::InitSafeGame()
{
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, FString::Printf(TEXT("Init Safe Game")));

    class UPLLSaveGame* SaveGameInstance = Cast<UPLLSaveGame>(UGameplayStatics::CreateSaveGameObject(UPLLSaveGame::StaticClass()));

    if(SaveGameInstance->IsValidLowLevel())
    {
        //Set the data i want to save
        SaveGameInstance->LastCheckpoint = MyLastCheckpoint;
        SaveGameInstance->PlayerName = "TestPlayer3000";
        SaveGameInstance->SaveSlotName = CurrentSaveSlotName;
        //Save to slot
        UGameplayStatics::SaveGameToSlot(SaveGameInstance, CurrentSaveSlotName, SaveGameInstance->UserIndex);
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, FString::Printf(TEXT("Saved Game. Last Checkpoint = %ls"), *MyLastCheckpoint->GetName()));
    }
    else
    {
        //if SaveGameObject not found create new one
        class UPLLSaveGame* SaveGameInstance_2 = Cast<UPLLSaveGame>(UGameplayStatics::CreateSaveGameObject(UPLLSaveGame::StaticClass()));

        if(!SaveGameInstance_2)
        {
            //probably sth going wrong
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, FString::Printf(TEXT("Failed To Safe Game")));
            return;
        }
        else
        {
            //Set the data i want to save
            SaveGameInstance_2->LastCheckpoint = MyLastCheckpoint;
            SaveGameInstance_2->PlayerName = "TestPlayer3000";
            SaveGameInstance_2->SaveSlotName = CurrentSaveSlotName;

            //Save to slot
            UGameplayStatics::SaveGameToSlot(SaveGameInstance_2, CurrentSaveSlotName, SaveGameInstance_2->UserIndex);
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, FString::Printf(TEXT("Saved Game. Last Checkpoint = %ls"), *SaveGameInstance_2->LastCheckpoint->GetName()));
        }
    }
}

void APLLGameModePlay::InitLoadGame()
{
    if(!GameLoaded)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, FString::Printf(TEXT("Init Load Game")));
    
        //check if everything can be found
        if(this->IsValidLowLevel())
        {
            if(UGameplayStatics::DoesSaveGameExist(CurrentSaveSlotName, 0))
            {
                DoesSaveGameExist = true;
                
                class UPLLSaveGame* LoadInstance = Cast<UPLLSaveGame>(UGameplayStatics::CreateSaveGameObject(UPLLSaveGame::StaticClass()));
    
                if(LoadInstance->IsValidLowLevel() && KaiCharacter->IsValidLowLevel())
                {
                    // TODO: change to actual saveGame Data
                    LoadInstance = Cast<UPLLSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSaveSlotName, 0));

                    if(LoadInstance->LastCheckpoint != nullptr)
                    {
                        // get loaded data
                        MyLastCheckpoint = LoadInstance->LastCheckpoint;
                    }
                    else
                    {
                        MyLastCheckpoint = DefaultStartCheckpoint;
                        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, FString::Printf(TEXT("No Checkpoint set in Loaded Game. Set to DefaultStartCheckpoint")));
                    }                        

                    if(!MyLastCheckpoint->IsValidLowLevel())
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, FString::Printf(TEXT("Failed to Load Game. Checkpoint not ready")));
                        return;
                    }
                    // apply data
                    if(!StartFromEditorLocation)
                    {
                        KaiCharacter->SetActorLocation(MyLastCheckpoint->GetActorLocation());
                    }
                    
                    GameLoaded = true;
    
                    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, FString::Printf(TEXT("LoadedGame. Last Checkpoint = %ls"), *MyLastCheckpoint->GetName()));
                }
                else
                {
                    class UPLLSaveGame* LoadInstance_2 = Cast<UPLLSaveGame>(UGameplayStatics::CreateSaveGameObject(UPLLSaveGame::StaticClass()));
    
                    if(!LoadInstance_2)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, FString::Printf(TEXT("Failed Load Game")));
                        return;
                    }
                    else
                    {
                        LoadInstance_2 = Cast<UPLLSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadInstance_2->SaveSlotName, LoadInstance_2->UserIndex));
                        // get loaded data
                        MyLastCheckpoint = LoadInstance_2->LastCheckpoint;

                        if(!StartFromEditorLocation)
                        {
                            // apply data
                            KaiCharacter->SetActorLocation(MyLastCheckpoint->GetActorLocation());
                        }
                        GameLoaded = true;
                        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, FString::Printf(TEXT("LoadedGame. Last Checkpoint = %ls"), *LoadInstance_2->LastCheckpoint->GetName()));
    
                    }
                        
                }
            }
            else
            {
                DoesSaveGameExist = false;
                GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, FString::Printf(TEXT("No SaveGame found ")));
            }
        }
            
    }

}

void APLLGameModePlay::SafeGameLogic()
{
    if(CanSaveGame && !IsPreparingSave)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, FString::Printf(TEXT("Can Safe Game")));

        InitSafeGame();

        SaveGameTimer = 0;
        CanSaveGame = false;
        IsPreparingSave = false;
    }
    else if(!IsPreparingSave)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, FString::Printf(TEXT("Prepare Safe Game")));

        TArray<AActor*> actorArray;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), actorArray);

        IsPreparingSave = true;
        CanSaveGame = true;
        for(AActor* actor : actorArray)
        {
            if(!actor->IsValidLowLevel())
            {
                CanSaveGame = false;
            }
        }
        IsPreparingSave = false;
        
        if(CanSaveGame)
        {
            InitSafeGame();
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, FString(TEXT("Failed To Prepare Safe Game. Safe failed")), true);
        }
    }
}

void APLLGameModePlay::LoadGameLogic(float dt)
{
    if(!GameLoaded)
    {
        loadingSequenceTimer += 1 * dt;

        if(loadingSequenceTimer > 0.20) // why 0.20 ?? -> to make sure all references have been found. But why 0.20 ?
        {
            InitLoadGame();

            loadingSequenceTimer = 0.f;
        }
    }
}

