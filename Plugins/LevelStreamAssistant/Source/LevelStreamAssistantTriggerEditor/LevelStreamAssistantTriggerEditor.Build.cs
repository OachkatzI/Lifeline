// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

using UnrealBuildTool;

public class LevelStreamAssistantTriggerEditor : ModuleRules
{
	public LevelStreamAssistantTriggerEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core" });

		// LevelStreamAssistant plugin can work with any actor to do the level streaming.
		// any object inheriting ULevelStreamingActorEditorBase is provided with an interface to work with the plugin.

		// This module is editor only and provides ability to edit level streaming with ALevelStreamingTrigger.
		// This module is seperated solely to show how to extend LevelStreamAssistant plugin.

		PrivateDependencyModuleNames.AddRange(new string[] { 
			"CoreUObject", 
			"Engine", 
			"UnrealEd",
			"LevelStreamAssistant",
			"LevelStreamAssistantTrigger"
		});
	}
}
