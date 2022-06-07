// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class Nearsoccer : ModuleRules
{
	public Nearsoccer(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "Niagara" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
		
		PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "OnlineSubsystemNull"});
		
		IncludeSubdirectoriesInIncludePaths();
	}
	
	private void IncludeSubdirectoriesInIncludePaths()
	{
		PrivateIncludePaths.Add(ModuleDirectory);
		AddDirectoriesRecursive(ModuleDirectory);
	}

	private void AddDirectoriesRecursive(string directoryPathToSearch)
	{
		foreach (string directoryPath in Directory.GetDirectories(directoryPathToSearch))
		{
			PrivateIncludePaths.Add(directoryPath);
			AddDirectoriesRecursive(directoryPath);
		}
	}
}
