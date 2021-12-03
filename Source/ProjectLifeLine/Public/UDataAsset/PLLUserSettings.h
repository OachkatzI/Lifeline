// Project LifeLine

#pragma once

#include "CoreMinimal.h"
#include <list>
#include <string>
#include "Engine/DataAsset.h"
#include "GenericPlatform/GenericWindow.h"
#include "PLLUserSettings.generated.h"

UENUM()
enum EPLLWindowTypeEnum
{
	NativeFullscreen UMETA(DisplayName = "NativeFullscreen"),
	MaximizedWindow UMETA(DisplayName = "MaximizedWindow"),
	Windowed UMETA(DisplayName = "Windowed"),
};

UCLASS(BlueprintType)
class PROJECTLIFELINE_API UPLLUserSettings : public UDataAsset
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Video")
	TEnumAsByte<EWindowMode::Type>  WindowType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Video")
	FIntPoint ScreenResolution;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Video")
	bool FullscreenModeActive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Video", meta = ( UIMin = "30", UIMax = "120"))
	int FrameRateLimit;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Video", meta = ( UIMin = "0", UIMax = "4"))
	int VideoQuality;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Video")
	float ResolutionScale;

	// TODO: is this needed in the user settings? for what?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Video")
	bool VerticalSync;

	// Not in GameUserSettings included
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	bool MotionBlurActive;

	// Not in GameUserSettings included
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	bool HelpSystemActive;

	// Not in GameUserSettings included
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	bool AimAssistActive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	int FieldOfView;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = ( UIMin = "0.0", UIMax = "1.0"))
	float MusicVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = ( UIMin = "0.0", UIMax = "1.0"))
	float SFXVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	bool MusicActive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	bool SFXActive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Controls")
	bool GamepadControlsActive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Controls")
	bool MouseKeyboardControlsActive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Saved Game")
	FName DefaultSaveSlotName;
	
public:

	UFUNCTION(BlueprintCallable)
    bool ResetUserSettings();

	UFUNCTION(BlueprintCallable)
	bool LoadAndApplyInitialSettings();
};
