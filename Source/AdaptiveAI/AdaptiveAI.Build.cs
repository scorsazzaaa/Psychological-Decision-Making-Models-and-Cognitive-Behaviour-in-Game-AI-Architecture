using UnrealBuildTool;
using System.IO;

public class AdaptiveAI : ModuleRules
{
    public AdaptiveAI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "AIModule",
            "GameplayTasks",
            "UMG",
            "Slate",
            "SlateCore",
            "Projects"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "NavigationSystem",
            "RHI",
            "RenderCore",
            "Json",
            "JsonUtilities"
        });

        // Use Path.Combine with relative paths to the project directory
        string EngineDir = Path.Combine("..", "..", "..", "Engine");
        string AIModuleInclude = Path.Combine(EngineDir, "Source", "Runtime", "AIModule", "Classes");

        PublicIncludePaths.AddRange(new string[]
        {
            "AdaptiveAI/Public",
            "AdaptiveAI/Public/Conversation",
            "AdaptiveAI/Public/NPC",
            "AdaptiveAI/Public/UI",
            AIModuleInclude
        });

        PrivateIncludePaths.AddRange(new string[]
        {
            "AdaptiveAI/Private",
            "AdaptiveAI/Private/Conversation",
            "AdaptiveAI/Private/NPC",
            "AdaptiveAI/Private/UI"
        });
    }
}
