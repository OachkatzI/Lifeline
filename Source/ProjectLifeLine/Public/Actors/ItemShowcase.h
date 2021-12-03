// Project LifeLine

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemShowcase.generated.h"


UENUM()
enum EItemShowcaseMode
{
	FadingIn,
	Showcasing,
	FadingOut
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNextItemShowcased);


UCLASS()
class PROJECTLIFELINE_API AItemShowcase : public AActor
{
	GENERATED_BODY()
	
public:	

	AItemShowcase();
	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnNextItemShowcased OnNextItemShowcased;
	
protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void ShowcaseNextItem();
	
	UPROPERTY(EditAnywhere, Category = "ItemShowcase")
	TArray<UStaticMesh*> ItemShowcaseList;

	UPROPERTY(EditAnywhere, Category = "ItemShowcase")
	float ShowcaseTimePerItem;

	UPROPERTY(EditAnywhere, Category = "ItemShowcase")
	float ItemTransitionTime;

	UPROPERTY(EditAnywhere, Category = "ItemShowcase")
	bool bShuffle;

	UPROPERTY(EditAnywhere, Category = "ItemShowcase")
	UCurveFloat* ItemFadeInOutCurve;

	UPROPERTY(EditAnywhere, Category = "ItemShowcase")
	float ItemRotationSpeed;

	UPROPERTY(EditAnywhere, Category = "ItemShowcase")
	FRotator AxisRotationWeight;

private:

	uint8 GetNextItemIndex() const;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* CurrentShowcaseItem;
	
	FTimerHandle ShowcaseTimer;

	uint8 CurrentShowcaseItemIndex;

	EItemShowcaseMode CurrentShowcasingMode;

	UFUNCTION()
	void StartItemFadeInOut();

	UFUNCTION()
	void EndFadingIn();

	FVector GetDesiredItemScale() const;

	float CurveMin;
	float CurveMax;
};
