// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PLLRopeAnchor.generated.h"


class APLLRope;
struct FRopeAttachmentParams;


UCLASS()
class PROJECTLIFELINE_API APLLRopeAnchor : public AActor
{
	GENERATED_BODY()

public:

	APLLRopeAnchor();
	
	void Attach(FRopeAttachmentParams Params);
	
	void Release();
	
	void SetRope(APLLRope* NewRope) { Rope = NewRope; }

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Component")
	UStaticMeshComponent* MeshComp;

	UPROPERTY()
	APLLRope* Rope;
};
