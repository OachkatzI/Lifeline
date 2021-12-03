using UnrealBuildTool;

public class UnrealAtomsEditor : ModuleRules
{
	public UnrealAtomsEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				"AssetTools",
				"MainFrame",
				//	"WorkspaceMenuStructure",
			});

		PrivateIncludePaths.AddRange(
			new string[]
			{
				"UnrealAtomsEditor/Private",
				"UnrealAtomsEditor/Private/AssetTools",
				"UnrealAtomsEditor/Private/Factories",
				// "UnrealAtomsEditor/Private/Shared",
				"UnrealAtomsEditor/Private/Styles",
				// "UnrealAtomsEditor/Private/Toolkits",
				// "UnrealAtomsEditor/Private/Widgets",
			});

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"ContentBrowser",
				"Core",
				"CoreUObject",
				"DesktopWidgets",
				"EditorStyle",
				"Engine",
				"InputCore",
				"Projects",
				"Slate",
				"SlateCore",
				"UnrealAtoms",
				"UnrealEd",
			});

		PrivateIncludePathModuleNames.AddRange(
			new string[]
			{
				"AssetTools",
				"UnrealEd",
				//"WorkspaceMenuStructure",
			});
	}
}