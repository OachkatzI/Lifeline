// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/PLLHUD.h"

#include <Actor.h>

#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "Characters/PLLCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SceneComponent/PLLCharacterSensorComponent.h"
#include "UObject/ConstructorHelpers.h"

APLLHUD::APLLHUD()
{
	
}

void APLLHUD::DrawHUD()
{
	Super::DrawHUD();

}

void APLLHUD::SetRopeAttachable(bool IsAttachable)
{
	//ignore if reduntant
	//TODO: make so programm only reaches this point if sth actually changed
	if(!CrosshairWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("PLLHUD: Widget missing?") );
		return;
	}
	
	if(IsAttachable == RopeIsAttachable )
	{
		return;
	}

	RopeIsAttachable = IsAttachable;

	//inform widget
	CrosshairWidget->RopeAttachable(RopeIsAttachable);
}


