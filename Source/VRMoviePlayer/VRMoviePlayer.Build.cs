// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VRMoviePlayer : ModuleRules
{
	public VRMoviePlayer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"HeadMountedDisplay",
			"MediaAssets",
			"MediaUtils",
			"HTTP",
			"Json",
			"JsonUtilities",
			"HTTPServer",
			"Sockets",
			"Networking"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		// For Oculus Quest support
		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			PrivateDependencyModuleNames.Add("OculusHMD");
		}
	}
}
