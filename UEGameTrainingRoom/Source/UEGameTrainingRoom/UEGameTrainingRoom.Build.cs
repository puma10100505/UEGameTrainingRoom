// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UEGameTrainingRoom : ModuleRules
{
	public UEGameTrainingRoom(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG" });

		PrivateIncludePaths.AddRange(new string[] {
			"UEGameTrainingRoom",
			"UEGameTrainingRoom/Characters",
			"UEGameTrainingRoom/Weapons",
			"UEGameTrainingRoom/UI",
			"UEGameTrainingRoom/Controllers",
			"UEGameTrainingRoom/Shared"
		});
	}
}
