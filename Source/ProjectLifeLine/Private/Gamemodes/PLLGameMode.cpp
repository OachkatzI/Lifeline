// Copyright Epic Games, Inc. All Rights Reserved.

#include "Gamemodes/PLLGameMode.h"
#include "UI/PLLHUD.h"

APLLGameMode::APLLGameMode() : Super()
{
	// use our custom HUD class
	HUDClass = APLLHUD::StaticClass();
}
