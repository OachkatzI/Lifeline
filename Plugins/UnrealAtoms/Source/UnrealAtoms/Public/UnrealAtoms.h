#pragma once

#include "CoreMinimal.h"

// Additional Includes
// #include "Styling/SlateStyle.h"
// #include "Styling/SlateStyleRegistry.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#if WITH_EDITOR
#include "Developer/AssetTools/Public/AssetTypeCategories.h"
#endif // WITH_EDITOR

class FUnrealAtomsModule : public IModuleInterface
{
public:

	/**
	* Singleton-like access to this module's interface.  This is just for convenience!
	* Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	*
	* @return Returns singleton instance, loading the module on demand if needed
	*/
	static FUnrealAtomsModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FUnrealAtomsModule>("UnrealAtoms");
	}


	/**
	* Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	*
	* @return True if the module is loaded and ready to use
	*/
	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("UnrealAtoms");
	}


	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

#if WITH_EDITOR
	virtual EAssetTypeCategories::Type GetAtomsAssetCategoryBit() const { return AtomsAssetCategoryBit; }
protected:
	EAssetTypeCategories::Type AtomsAssetCategoryBit;
#endif
};
