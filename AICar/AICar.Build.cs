// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AICar : ModuleRules
{
	public AICar(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore", "PhysXVehicles", "HeadMountedDisplay", "WebSockets", "Json", "JsonUtilities" });

		PublicDefinitions.Add("HMD_MODULE_INCLUDED=1");
	}
}
