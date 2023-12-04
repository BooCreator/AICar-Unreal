// YourModuleName.Build.cs
using UnrealBuildTool;

public class MyWebSockets : ModuleRules
{
    public MyWebSockets(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] { "Core", "HTTP" });
        PrivateIncludePaths.AddRange( new string[] { "MyWebSockets/Private" } );
        AddEngineThirdPartyPrivateStaticDependencies(Target, "zlib");
        AddEngineThirdPartyPrivateStaticDependencies(Target, "libWebSockets");
    }
}