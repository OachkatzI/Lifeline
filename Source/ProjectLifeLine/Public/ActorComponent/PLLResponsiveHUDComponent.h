// Project LifeLine

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UI/PLLHUD.h"

#include "PLLResponsiveHUDComponent.generated.h"


class UPLLCharacterSensorComponent;
class APLLCharacter;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTLIFELINE_API UPLLResponsiveHUDComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPLLResponsiveHUDComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(BlueprintReadWrite)
	UPLLCharacterSensorComponent* Sensor;
	
private:
	APLLHUD* MyHUD;
};
