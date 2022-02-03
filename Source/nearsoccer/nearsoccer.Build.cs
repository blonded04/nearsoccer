// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class nearsoccer : ModuleRules
{
	public nearsoccer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
