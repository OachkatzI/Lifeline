// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "SelectionTool/ActorSelectorBox.h"

AActorSelectorBox::AActorSelectorBox()
{
	PrimaryActorTick.bCanEverTick = false;

	bIsEditorOnlyActor = true;
	bRelevantForLevelBounds = false;
	bNetLoadOnClient = false;
	bEnableAutoLODGeneration = false;
	SetActorHiddenInGame(true);
	SetCanBeDamaged(false);

	BoxComponent = CreateDefaultSubobject<UActorSelectorBoxComponent>("SelectionBox");
	SetRootComponent(BoxComponent);
}

UActorSelectorBoxComponent::UActorSelectorBoxComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bIsEditorOnly = true;
	AlwaysLoadOnClient = false;
	AlwaysLoadOnServer = false;
	bEnableAutoLODGeneration = false;
	bVisualizeComponent = true;

	ShapeColor = FColor(50, 111, 230);
	LineThickness = 5.f;

	SetCanEverAffectNavigation(false);

	InitBoxExtent(FVector(100.f));
}

UActorSelectorBoxFactory::UActorSelectorBoxFactory()
{
	NewActorClass = AActorSelectorBox::StaticClass();
}
