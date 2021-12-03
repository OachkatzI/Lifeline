// Project LifeLine


#include "UDataAsset/PLLUserSettings.h"
#include "Gamemodes/PLLGameUserSettings.h"
#include "GameFramework/GameUserSettings.h"

bool UPLLUserSettings::ResetUserSettings()
{
	UPLLGameUserSettings* MyGameUserSettings = UPLLGameUserSettings::GetPLLGameUserSettings();

	if(!MyGameUserSettings)
	{
		return false;
	}

	// Video
	MyGameUserSettings->SetFullscreenMode(WindowType);
	MyGameUserSettings->SetScreenResolution(ScreenResolution);
	MyGameUserSettings->SetFrameRateLimit(FrameRateLimit);
	MyGameUserSettings->SetOverallScalabilityLevel(VideoQuality);//0:low, 1:medium, 2:high, 3:epic, 4:cinematic
	MyGameUserSettings->SetResolutionScaleNormalized(ResolutionScale);
	MyGameUserSettings->SetVSyncEnabled(VerticalSync);
	
	//Gameplay
	MyGameUserSettings->SetMotionBlurActive(MotionBlurActive);// activate in scene 
	MyGameUserSettings->SetPLLHelpSystemActive(HelpSystemActive);// activate in scene 
	MyGameUserSettings->SetPLLAimAssistActive(AimAssistActive);// activate in scene 
	MyGameUserSettings->SetFieldOfView(FieldOfView);// activate in scene 
	
	//Audio
	MyGameUserSettings->SetMusicVolume(MusicVolume);// set future AudioController
	MyGameUserSettings->SetSFXVolume(SFXVolume);// set future AudioController
	MyGameUserSettings->SetMusicActive(MusicActive);// set future AudioController
	MyGameUserSettings->SetSFXActive(SFXActive);// set future AudioController
	
	//Controls
	MyGameUserSettings->SetGamepadControlsActive(GamepadControlsActive);
	MyGameUserSettings->SetMouseKeyboardControlsActive(MouseKeyboardControlsActive);

	MyGameUserSettings->ValidateSettings();
	MyGameUserSettings->ApplySettings(false);
	return true;
}

bool UPLLUserSettings::LoadAndApplyInitialSettings()
{
	UPLLGameUserSettings* MyGameUserSettings = UPLLGameUserSettings::GetPLLGameUserSettings();

	if(!MyGameUserSettings)
	{
		return false;
	}

	// Set Fullscreen Mode
	auto MyFullScreenMode = MyGameUserSettings->GetFullscreenMode() ?  MyGameUserSettings->GetFullscreenMode() : WindowType;
	MyGameUserSettings->SetFullscreenMode(static_cast<EWindowMode::Type>(MyFullScreenMode));

	// Set Screen Resolution
	FIntPoint MyScreenResolution = MyGameUserSettings->GetScreenResolution() != FIntPoint::ZeroValue ? MyGameUserSettings->GetScreenResolution() : ScreenResolution;
	MyGameUserSettings->SetScreenResolution(MyScreenResolution);

	// TODO: Apply rest of the settings? here or in PLLGameUserSettings?
	
	// Validate Settings
	MyGameUserSettings->ValidateSettings();
	MyGameUserSettings->ApplySettings(false);
	return true;
}


