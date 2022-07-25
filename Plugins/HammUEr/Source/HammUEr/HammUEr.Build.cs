// HammUEr
// (c) 2015-2021 Turfster / NT Entertainment

using UnrealBuildTool;

public class HammUEr : ModuleRules
{
    public HammUEr(ReadOnlyTargetRules Target) : base(Target)
    {
    
        PublicIncludePaths.AddRange(
            new string[] {
                // ... add other public include paths required here ...
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                "HammUEr/Public",
                "HammUEr/Private",
                "HammUEr/Classes",
				
				// ... add other private include paths required here ...
			}
            );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "HammUErRuntime",
                "Engine",
                "Core"
				// ... add other public dependencies that you statically link with here ...
			}
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "AppFramework",
                "AssetRegistry",
                "Core",
                "CoreUObject",
                "DesktopPlatform",
                "DeveloperSettings",                
                "EditorStyle",
                "Engine",
                "Foliage",
                "GraphEditor",
                "HammUErRuntime",
                "ImageWrapper",
                "InputCore",
                "Landscape",
                "LandscapeEditor",
                "LevelEditor",
                "MainFrame",
                "MaterialUtilities",
                "MeshUtilities",
                "Projects",
                "PropertyEditor",
                "RHI",
                "RawMesh",
                "RenderCore",
                "Slate",
                "SlateCore",
                "SourceControl",
                "SourceControlWindows",
                "UnrealEd",
                "WorldBrowser"
            }
            );


        PrivateIncludePathModuleNames.AddRange(
        new string[] {
                "AssetRegistry",
                "AssetTools",
                "ContentBrowser",
                "Landscape",
                "MeshUtilities",
                "MaterialUtilities",
           }
        );

        DynamicallyLoadedModuleNames.AddRange(
          new string[] {
               // "AssetRegistry",
                "AssetTools",
                "SceneOutliner",
                //"MeshUtilities",
                "ContentBrowser",
          }
        );

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.Add(ModuleDirectory + "/../../Libs/HammUErLib.lib");
            PublicAdditionalLibraries.Add(ModuleDirectory + "/../../Libs/VTFLib.lib");
        }
        else
        {
            throw new BuildException("Platform not supported by HammUEr.");
        }

        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				
				// ... add any modules that your module loads dynamically here ...
			}
            );
    }
}
