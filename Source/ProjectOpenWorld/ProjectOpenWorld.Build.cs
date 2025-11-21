// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectOpenWorld : ModuleRules
{
	public ProjectOpenWorld(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Landscape", "UMG",
            "AIModule", "SlateCore",

        });

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "UnrealEd",             // GEditor, 에디터 관련
                    "EditorSubsystem",      // UEditorAssetSubsystem
                    "EditorScriptingUtilities" // 필요 시
                }
            );
        }
    }
}
