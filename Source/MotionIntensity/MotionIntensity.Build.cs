// Copyright (c) 2024 Tyoma Makeev

using UnrealBuildTool;

public class MotionIntensity : ModuleRules
{
	public MotionIntensity(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine"
			}
		);
	}
}
