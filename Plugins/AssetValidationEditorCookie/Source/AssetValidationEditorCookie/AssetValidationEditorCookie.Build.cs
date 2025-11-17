// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AssetValidationEditorCookie : ModuleRules
{
	public AssetValidationEditorCookie(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject", 
				"Engine",
				"UnrealEd",
				"InterchangeCore",           // Base Interchange
				"InterchangeEngine",        // Engine integration
				"InterchangeNodes",         // Base nodes
				"InterchangeFactoryNodes",  // Factory nodes (OBLIGATOIRE pour UInterchangeMeshFactoryNode)
				"InterchangeCommonParser",  // Parser commun
				"InterchangeImport", "DataValidation"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore", 
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
