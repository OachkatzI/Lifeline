#include "Containers/Array.h"
#include "ISettingsModule.h"
// #include "ISettingsSection.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Templates/SharedPointer.h"
#include "Toolkits/AssetEditorToolkit.h"

#include "Styles/UnrealAtomsEditorStyle.h"
#include "AssetTools/AtomEditorAction.h"
#include "Variables/AtomBoolVariable.h"
#include "Variables/AtomFloatVariable.h"
#include "Variables/AtomFColorVariable.h"
#include "Variables/AtomFRotatorVariable.h"
#include "Variables/AtomFStringVariable.h"
#include "Variables/AtomFVectorVariable.h"
#include "Variables/AtomIntVariable.h"
#include "Events/AtomVoidEvent.h"


#define LOCTEXT_NAMESPACE "FUnrealAtomsEditorModule"

/**
 * Implements the UnrealAtomsEditor module.
 */
class FUnrealAtomsEditorModule
	: public IHasMenuExtensibility
	  , public IHasToolBarExtensibility
	  , public IModuleInterface
{
public:

	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override
	{
		return MenuExtensibilityManager;
	}


	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override
	{
		return ToolBarExtensibilityManager;
	}


	//~ IModuleInterface interface

	virtual void StartupModule() override
	{
		Style = MakeShareable(new FUnrealAtomsEditorStyle());

		//		FUnrealAtomsEditorCommands::Register();

		RegisterAssetTools();
		// RegisterMenuExtensions();
		// RegisterSettings();
	}


	virtual void ShutdownModule() override
	{
		UnregisterAssetTools();
		// UnregisterMenuExtensions();
		// UnregisterSettings();
	}


	virtual bool SupportsDynamicReloading() override
	{
		return true;
	}


protected:


	/** Registers asset tool actions. */
	void RegisterAssetTools()
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

		//Variables
		FText  AtomSubMenu      = NSLOCTEXT("AssetTypeActions", "AssetTypeActions_UnrealAtomsVariableSubmenu", "Variables");
		FColor AtomDisplayColor = FColor::Green;

		FText AtomName = NSLOCTEXT("AssetTypeActions", "AssetTypeActions_UnrealAtomsBoolVariable", "Bool Variable");
		RegisterAssetTypeAction<UAtomBoolVariable>(AssetTools, AtomName, AtomSubMenu, AtomDisplayColor);

		AtomName = NSLOCTEXT("AssetTypeActions", "AssetTypeActions_UnrealAtomsFloatVariable", "Float Variable");
		RegisterAssetTypeAction<UAtomFloatVariable>(AssetTools, AtomName, AtomSubMenu, AtomDisplayColor);

		AtomName = NSLOCTEXT("AssetTypeActions", "AssetTypeActions_UnrealAtomsFRotatorVariable", "FRotator Variable");
		RegisterAssetTypeAction<UAtomFRotatorVariable>(AssetTools, AtomName, AtomSubMenu, AtomDisplayColor);

		AtomName = NSLOCTEXT("AssetTypeActions", "AssetTypeActions_UnrealAtomsFColorVariable", "FColor Variable");
		RegisterAssetTypeAction<UAtomFColorVariable>(AssetTools, AtomName, AtomSubMenu, AtomDisplayColor);

		AtomName = NSLOCTEXT("AssetTypeActions", "AssetTypeActions_UnrealAtomsFStringVariable", "FString Variable");
		RegisterAssetTypeAction<UAtomFStringVariable>(AssetTools, AtomName, AtomSubMenu, AtomDisplayColor);

		AtomName = NSLOCTEXT("AssetTypeActions", "AssetTypeActions_UnrealAtomsFVectorVariable", "FVector Variable");
		RegisterAssetTypeAction<UAtomFVectorVariable>(AssetTools, AtomName, AtomSubMenu, AtomDisplayColor);

		AtomName = NSLOCTEXT("AssetTypeActions", "AssetTypeActions_UnrealAtomsIntVariable", "Int Variable");
		RegisterAssetTypeAction<UAtomIntVariable>(AssetTools, AtomName, AtomSubMenu, AtomDisplayColor);


		// // Events
		// AtomSubMenu      = NSLOCTEXT("AssetTypeActions", "AssetTypeActions_UnrealAtomsEventSubmenu", "Events");
		// AtomDisplayColor = FColor::Red;
		//
		// AtomName = NSLOCTEXT("AssetTypeActions", "AssetTypeActions_UnrealAtomsVoidEvent", "Void Event");
		// RegisterAssetTypeAction<UAtomVoidEvent>(AssetTools, AtomName, AtomSubMenu, AtomDisplayColor);
		//
		// AtomName = NSLOCTEXT("AssetTypeActions", "AssetTypeActions_UnrealAtomsBoolEvent", "Bool Event");
		// RegisterAssetTypeAction<UAtomBoolEvent>(AssetTools, AtomName, AtomSubMenu, AtomDisplayColor);
		//
		// AtomName = NSLOCTEXT("AssetTypeActions", "AssetTypeActions_UnrealAtomsFloatEvent", "Float Event");
		// RegisterAssetTypeAction<UAtomFloatEvent>(AssetTools, AtomName, AtomSubMenu, AtomDisplayColor);
		//
		// AtomName = NSLOCTEXT("AssetTypeActions", "AssetTypeActions_UnrealAtomsFRotatorEvent", "FRotator Event");
		// RegisterAssetTypeAction<UAtomFRotatorEvent>(AssetTools, AtomName, AtomSubMenu, AtomDisplayColor);
		//
		// AtomName = NSLOCTEXT("AssetTypeActions", "AssetTypeActions_UnrealAtomsFStringEvent", "FString Event");
		// RegisterAssetTypeAction<UAtomFStringEvent>(AssetTools, AtomName, AtomSubMenu, AtomDisplayColor);
		//
		// AtomName = NSLOCTEXT("AssetTypeActions", "AssetTypeActions_UnrealAtomsFVectorEvent", "FVector Event");
		// RegisterAssetTypeAction<UAtomFVectorEvent>(AssetTools, AtomName, AtomSubMenu, AtomDisplayColor);
		//
		// AtomName = NSLOCTEXT("AssetTypeActions", "AssetTypeActions_UnrealAtomsIntEvent", "Int Event");
		// RegisterAssetTypeAction<UAtomIntEvent>(AssetTools, AtomName, AtomSubMenu, AtomDisplayColor);
	}


	// /**
	//  * Registers a single asset type action.
	//  *
	//  * @param AssetTools The asset tools object to register with.
	//  * @param Action The asset type action to register.
	//  */
	// void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
	// {
	// 	AssetTools.RegisterAssetTypeActions(Action);
	// 	RegisteredAssetTypeActions.Add(Action);
	// }


	template <typename T>
	void RegisterAssetTypeAction(IAssetTools& AssetTools, const FText AtomName, const FText AtomSubMenu, FColor AtomDisplayColor)
	{
		const TSharedRef<IAssetTypeActions> Action = MakeShareable(new FAtomEditorAction<T>(Style.ToSharedRef(), AtomName, AtomSubMenu,
		                                                                                    AtomDisplayColor));
		AssetTools.RegisterAssetTypeActions(Action);
		RegisteredAssetTypeActions.Add(Action);
	}


	/** Unregisters asset tool actions. */
	void UnregisterAssetTools()
	{
		FAssetToolsModule* AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools");

		if (AssetToolsModule != nullptr)
		{
			IAssetTools& AssetTools = AssetToolsModule->Get();

			for (auto Action : RegisteredAssetTypeActions)
			{
				AssetTools.UnregisterAssetTypeActions(Action);
			}
		}
	}


	// // /** Register the text asset editor settings. */
	// void RegisterSettings()
	// {
	// 	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	//
	// 	if (SettingsModule != nullptr)
	// 	{
	// 		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Editor", "Plugins", "UnrealAtoms",
	// 		                                                                       LOCTEXT("UnrealAtomsSettingsName", "Unreal Atoms"),
	// 		                                                                       LOCTEXT("UnrealAtomsSettingsDescription",
	// 		                                                                               "Configure the Unreal Atoms plug-in."),
	// 		                                                                       GetMutableDefault<UUnrealAtomsEditorSettings>()
	// 		                                                                      );
	// 	}
	// }
	//
	//
	// /** Unregister the text asset editor settings. */
	// void UnregisterSettings()
	// {
	// 	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	//
	// 	if (SettingsModule != nullptr)
	// 	{
	// 		SettingsModule->UnregisterSettings("Editor", "Plugins", "UnrealAtoms");
	// 	}
	// }


protected:

	// /** Registers main menu and tool bar menu extensions. */
	// void RegisterMenuExtensions()
	// {
	// 	MenuExtensibilityManager    = MakeShareable(new FExtensibilityManager);
	// 	ToolBarExtensibilityManager = MakeShareable(new FExtensibilityManager);
	// }
	//
	//
	// /** Unregisters main menu and tool bar menu extensions. */
	// void UnregisterMenuExtensions()
	// {
	// 	MenuExtensibilityManager.Reset();
	// 	ToolBarExtensibilityManager.Reset();
	// }
private:

	/** Holds the menu extensibility manager. */
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;

	/** The collection of registered asset type actions. */
	TArray<TSharedRef<IAssetTypeActions>> RegisteredAssetTypeActions;

	/** Holds the plug-ins style set. */
	TSharedPtr<ISlateStyle> Style;

	/** Holds the tool bar extensibility manager. */
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;
};


IMPLEMENT_MODULE(FUnrealAtomsEditorModule, UnrealAtomsEditor);


#undef LOCTEXT_NAMESPACE
