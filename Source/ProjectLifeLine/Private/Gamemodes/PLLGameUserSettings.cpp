// Project LifeLine


#include "Gamemodes/PLLGameUserSettings.h"

void UPLLGameUserSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
	// same GameUserSettings
	ApplyResolutionSettings(bCheckForCommandLineOverrides);
	ApplyNonResolutionSettings();

	// apply additional settings
	//ApplyAudioSettings();
	
	// same GameUserSettings
	RequestUIUpdate();

	SaveSettings();
}

UPLLGameUserSettings*  UPLLGameUserSettings::GetPLLGameUserSettings()
{
	return static_cast<UPLLGameUserSettings*>(GEngine->GetGameUserSettings());
}

void UPLLGameUserSettings::ApplyAudioSettings()
{
	// TODO: Apply settings to audio system
}

bool UPLLGameUserSettings::GetMotionBlurActive()
{
	return MotionBlurActive;
}

void UPLLGameUserSettings::SetMotionBlurActive(bool IsActive)
{
	MotionBlurActive = IsActive;
}

float UPLLGameUserSettings::GetFieldOfView()
{
	return FieldOfView;
}

void UPLLGameUserSettings::SetFieldOfView(float FoV)
{
	FieldOfView = FoV;
}

bool UPLLGameUserSettings::GetPLLHelpSystemActive()
{
	return PLLHelpSystemActive;
}

void UPLLGameUserSettings::SetPLLHelpSystemActive(bool IsActive)
{
	PLLHelpSystemActive = IsActive;
}

bool UPLLGameUserSettings::GetPLLAimAssistActive()
{
	return PLLAimAssistActive;
}

void UPLLGameUserSettings::SetPLLAimAssistActive(bool IsActive)
{
	PLLAimAssistActive = IsActive;
}

float UPLLGameUserSettings::GetMusicVolume()
{
	return MusicVolume;
}

void UPLLGameUserSettings::SetMusicVolume(float NewMusicVolume)
{
	MusicVolume = NewMusicVolume;
}

float UPLLGameUserSettings::GetSFXVolume()
{
	return SFXVolume;
}

void UPLLGameUserSettings::SetSFXVolume(float NewSFXVolume)
{
	SFXVolume = NewSFXVolume;
}

bool UPLLGameUserSettings::GetMusicActive()
{
	return MusicActive;
}

void UPLLGameUserSettings::SetMusicActive(bool IsActive)
{
	MusicActive = IsActive;
}

bool UPLLGameUserSettings::GetSFXActive()
{
	return SFXActive;
}

void UPLLGameUserSettings::SetSFXActive(bool IsActive)
{
	SFXActive = IsActive;
}

bool UPLLGameUserSettings::GetGamepadControlsActive()
{
	return GamepadControlsActive;
}

void UPLLGameUserSettings::SetGamepadControlsActive(bool IsActive)
{
	GamepadControlsActive = IsActive;
}

bool UPLLGameUserSettings::GetMouseKeyboardControlsActive()
{
	return MouseKeyboardControlsActive;
}

void UPLLGameUserSettings::SetMouseKeyboardControlsActive(bool IsActive)
{
	MouseKeyboardControlsActive = IsActive;
}

FName UPLLGameUserSettings::GetSelectedSaveSlotName() const
{
	return SelectedSaveSlotName;
}

void UPLLGameUserSettings::SetSelectedSaveSlotName(const FName& NewSelectedSaveSlotName)
{
	this->SelectedSaveSlotName = NewSelectedSaveSlotName;
}

void UPLLGameUserSettings::AddSaveSlotName(const FName& NewSaveSlotName)
{
	SaveSlotNameArray.Add(NewSaveSlotName);
}

void UPLLGameUserSettings::RemoveSaveSlotName(const FName& RemoveSaveSlotName)
{
	SaveSlotNameArray.Remove(RemoveSaveSlotName);
}


TArray<FName> UPLLGameUserSettings::GetSaveSlotNameArray() const
{
	return SaveSlotNameArray;
}

FString UPLLGameUserSettings::GenerateNewSaveSlotName()
{
	auto now = FDateTime::Now();
	
	FString string = "SaveSlot_" + FString::FromInt(now.GetDay()) + "_" + FString::FromInt(now.GetMonth()) + "_" + FString::FromInt(now.GetYear()) + "_"
	+ FString::FromInt(now.GetHour()) + "_" + FString::FromInt(now.GetMinute()) + "_" + FString::FromInt(now.GetSecond());
	
	return string;
}
