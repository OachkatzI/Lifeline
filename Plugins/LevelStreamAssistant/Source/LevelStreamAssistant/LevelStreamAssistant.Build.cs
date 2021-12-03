// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

using UnrealBuildTool;

public class LevelStreamAssistant : ModuleRules
{
	public LevelStreamAssistant(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core" });

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			"InputCore",
			"UnrealEd",
			"Json",
			"AppFramework", // required for color picker

			"EditorStyle",
			"Projects",
			"AssetRegistry",

			"LevelEditor",
			"PropertyEditor",
			"AssetManagerEditor", // required for SizeMap
			"ActorPickerMode",

			// rendering
			"RHI",
			"RenderCore",
		});
	}
}
