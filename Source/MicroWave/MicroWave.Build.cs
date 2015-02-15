// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MicroWave : ModuleRules
{
	public MicroWave(TargetInfo Target)
	{
        // Turn off Unity for faster compilation
        bFasterWithoutUnity = true;
        MinFilesUsingPrecompiledHeaderOverride = 1;

		PublicDependencyModuleNames.AddRange(
            new string[] { 
                "Core", 
                "CoreUObject", 
                "Engine", 
                "InputCore",
                "RHI",
                "RenderCore",
                "ShaderCore",
                "AIModule"
            });

        DynamicallyLoadedModuleNames.AddRange(
            new string[] {
                "CableComponent"
            });

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "CableComponent"
            });
	}
}
