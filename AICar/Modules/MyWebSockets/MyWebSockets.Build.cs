using UnrealBuildTool; 

public class MyWebSockets: ModuleRules { 
   
    protected virtual bool PlatformSupportsLibWebsockets
	{
		get {
			return
				Target.Platform == UnrealTargetPlatform.Win32 ||
				Target.Platform == UnrealTargetPlatform.Win64;
		}
	}

	public MyWebSockets(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange( new string[] { "Core", "HTTP" } );

		if (PlatformSupportsLibWebsockets)
		{
			PrivateIncludePaths.AddRange(
				new string[] { "Modules/MyWebSockets/Private" }
			);
			AddEngineThirdPartyPrivateStaticDependencies(Target, "libWebSockets");
		}
	}
}
