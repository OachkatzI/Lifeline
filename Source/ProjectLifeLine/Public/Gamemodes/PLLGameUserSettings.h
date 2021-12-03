// Project LifeLine

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "PLLGameUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTLIFELINE_API UPLLGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:

    virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;

	UFUNCTION(BlueprintCallable, Category=Settings)
	static UPLLGameUserSettings* GetPLLGameUserSettings();

	UFUNCTION(BlueprintCallable, Category=Settings)
	void ApplyAudioSettings();
	
	UFUNCTION(BlueprintCallable)
	bool GetMotionBlurActive();

	UFUNCTION(BlueprintCallable)
	void SetMotionBlurActive(bool IsActive);

	UFUNCTION(BlueprintCallable)
	float GetFieldOfView();

	UFUNCTION(BlueprintCallable)
	void SetFieldOfView(float FoV);

	UFUNCTION(BlueprintCallable)
	bool GetPLLHelpSystemActive();

	UFUNCTION(BlueprintCallable)
	void SetPLLHelpSystemActive(bool IsActive);

	UFUNCTION(BlueprintCallable)
	bool GetPLLAimAssistActive();

	UFUNCTION(BlueprintCallable)
	void SetPLLAimAssistActive(bool IsActive);

	UFUNCTION(BlueprintCallable)
	float GetMusicVolume();

	UFUNCTION(BlueprintCallable)
	void SetMusicVolume(float NewMusicVolume);

	UFUNCTION(BlueprintCallable)
	float GetSFXVolume();

	UFUNCTION(BlueprintCallable)
	void SetSFXVolume(float NewSFXVolume);

	UFUNCTION(BlueprintCallable)
	bool GetMusicActive();

	UFUNCTION(BlueprintCallable)
	void SetMusicActive(bool IsActive);

	UFUNCTION(BlueprintCallable)
	bool GetSFXActive();

	UFUNCTION(BlueprintCallable)
	void SetSFXActive(bool IsActive);

	UFUNCTION(BlueprintCallable)
	bool GetGamepadControlsActive();

	UFUNCTION(BlueprintCallable)
	void SetGamepadControlsActive(bool IsActive);

	UFUNCTION(BlueprintCallable)
	bool GetMouseKeyboardControlsActive();

	UFUNCTION(BlueprintCallable)
	void SetMouseKeyboardControlsActive(bool IsActive);

	UFUNCTION(BlueprintCallable)
	FName GetSelectedSaveSlotName() const;

	UFUNCTION(BlueprintCallable)
	void SetSelectedSaveSlotName(const FName& NewSelectedSaveSlotName);

	UFUNCTION(BlueprintCallable)
    void AddSaveSlotName(const FName& NewSaveSlotName);

	UFUNCTION(BlueprintCallable)
    void RemoveSaveSlotName(const FName& RemoveSaveSlotName);

	UFUNCTION(BlueprintCallable)
	TArray<FName> GetSaveSlotNameArray() const;

	UFUNCTION(BlueprintCallable)
	FString GenerateNewSaveSlotName();
	
	UPROPERTY(config, BlueprintReadWrite)
	TArray<FName> SaveSlotNameArray;

protected:

	UPROPERTY(config)
	bool MotionBlurActive;
	
	UPROPERTY(config)
	float FieldOfView;
	
	UPROPERTY(config)
	bool PLLHelpSystemActive;
	
	UPROPERTY(config)
	bool PLLAimAssistActive;

	UPROPERTY(config)
	float MusicVolume;

	UPROPERTY(config)
	float SFXVolume;

	UPROPERTY(config)
	bool MusicActive;

	UPROPERTY(config)
	bool SFXActive;

	UPROPERTY(config)
	bool GamepadControlsActive;

	UPROPERTY(config)
	bool MouseKeyboardControlsActive;
	
	UPROPERTY(config)
	FName SelectedSaveSlotName;
};
