// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UEGameTrainingRoom : ModuleRules
{
	public UEGameTrainingRoom(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "GameplayAbilities" });

		PrivateDependencyModuleNames.AddRange(new string[]{
			"GameplayAbilities",
			"GameplayTasks",
			"GameplayTags"

		});

		PrivateIncludePaths.AddRange(new string[] {
			"UEGameTrainingRoom",
			"UEGameTrainingRoom/Characters",
			"UEGameTrainingRoom/Characters/Attributes",
			"UEGameTrainingRoom/Weapons",
			"UEGameTrainingRoom/UI",
			"UEGameTrainingRoom/Controllers",
			"UEGameTrainingRoom/Shared",
			"UEGameTrainingRoom/AI",
			"UEGameTrainingRoom/Abilities",
			"UEGameTrainingRoom/Components",
			"UEGameTrainingRoom/States" 
		});
	}
}
