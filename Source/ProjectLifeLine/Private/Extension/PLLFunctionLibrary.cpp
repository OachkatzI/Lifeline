// Project LifeLine

#include "Extension/PLLFunctionLibrary.h"

#include "GeneralProjectSettings.h"


bool UPLLFunctionLibrary::IsGamePadConnected()
{
	auto genericApplication = FSlateApplication::Get().GetPlatformApplication();
	if (genericApplication.Get() != nullptr && genericApplication->IsGamepadAttached())
	{
		return true;
	}
	return false;
}


void UPLLFunctionLibrary::GetProjectVersion(FString& Version)
{
	Version = *GetDefault<UGeneralProjectSettings>()->ProjectVersion;
}
