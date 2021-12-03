// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectLifeLine : ModuleRules
{
	public ProjectLifeLine(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "CableComponent", "UnrealAtoms",
			"EngineSettings", "UINavigation"
		});
		PrivateDependencyModuleNames.AddRange(new string[]
			{"CableComponent", "Slate", "SlateCore", "UnrealAtoms", "UINavigation"});
	}
}