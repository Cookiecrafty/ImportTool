// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ImportTool : ModuleRules
{
	public ImportTool(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ImportTool",
			"ImportTool/Variant_Platforming",
			"ImportTool/Variant_Platforming/Animation",
			"ImportTool/Variant_Combat",
			"ImportTool/Variant_Combat/AI",
			"ImportTool/Variant_Combat/Animation",
			"ImportTool/Variant_Combat/Gameplay",
			"ImportTool/Variant_Combat/Interfaces",
			"ImportTool/Variant_Combat/UI",
			"ImportTool/Variant_SideScrolling",
			"ImportTool/Variant_SideScrolling/AI",
			"ImportTool/Variant_SideScrolling/Gameplay",
			"ImportTool/Variant_SideScrolling/Interfaces",
			"ImportTool/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
