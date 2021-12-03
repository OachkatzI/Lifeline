// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

using UnrealBuildTool;

public class LevelStreamAssistantTrigger : ModuleRules
{
	public LevelStreamAssistantTrigger(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core" });

		// This is a runtime module containing ALevelStreamingTrigger to support level streaming with triggers.

		// This is the only module that gets packaged with your game.

		// editor module LevelStreamingTriggerEditor gives ability to edit level streaming with ALevelStreamingTrigger.

		PrivateDependencyModuleNames.AddRange(new string[] { "CoreUObject", "Engine" });
	}
}
