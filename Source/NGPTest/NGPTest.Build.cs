// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NGPTest : ModuleRules
{
	public NGPTest(ReadOnlyTargetRules Target) : base(Target)
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
			"NGPTest",
			"NGPTest/Variant_Platforming",
			"NGPTest/Variant_Platforming/Animation",
			"NGPTest/Variant_Combat",
			"NGPTest/Variant_Combat/AI",
			"NGPTest/Variant_Combat/Animation",
			"NGPTest/Variant_Combat/Gameplay",
			"NGPTest/Variant_Combat/Interfaces",
			"NGPTest/Variant_Combat/UI",
			"NGPTest/Variant_SideScrolling",
			"NGPTest/Variant_SideScrolling/AI",
			"NGPTest/Variant_SideScrolling/Gameplay",
			"NGPTest/Variant_SideScrolling/Interfaces",
			"NGPTest/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
