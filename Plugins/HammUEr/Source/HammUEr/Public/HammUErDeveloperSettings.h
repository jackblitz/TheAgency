// HammUEr
// HammUEr Settings rework
// Copyright 2021 Turfster / NT Entertainment & Luna Ryuko
// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


#include "Engine/DeveloperSettings.h"
//#include "HammUEr/HammUErProcessor.h"
#include "Runtime/Engine/Classes/Engine/StaticMeshActor.h"
#include "UObject/Object.h"
#include "HammUErDeveloperSettings.generated.h"

// LUNA: This must match VTFLib!! I mass search replaced the names to have them look nicely in editor
// Also, I've hidden some types because they're either not useful or impossible to get
UENUM(BlueprintType)
namespace EVTFImageFormat
{
	enum Type
	{
		RGBA8888 = 0,
		ABGR8888,
		RGB888,
		BGR888,
		RGB565,
		I8					UMETA(Hidden),
		IA88				UMETA(Hidden),
		P8					UMETA(Hidden), // Not supported
		A8					UMETA(Hidden),
		RGB888_BLUESCREEN	UMETA(Hidden), // What's a bluescreen format????
		BGR888_BLUESCREEN	UMETA(Hidden),
		ARGB8888,
		BGRA8888,
		DXT1,
		DXT3,
		DXT5,
		BGRX8888,
		BGR565,
		BGRX5551,
		BGRA4444,
		DXT1_ONEBITALPHA,
		BGRA5551,
		UV88			UMETA(Hidden), // what's this texture format??
		UVWQ8888		UMETA(Hidden),
		RGBA16161616F	UMETA(Hidden), // Pointless, texture data is RGB8 (though we could add support for this)
		RGBA16161616	UMETA(Hidden), // same as above
		UVLX8888		UMETA(Hidden), // what's this texture format??
		R32F			UMETA(Hidden), // Impossible to get a 32-bit per channel texture source
		RGB323232F		UMETA(Hidden), // same as above
		RGBA32323232F   UMETA(Hidden), // same as above
		NV_DST16		UMETA(Hidden), // Vendor specific begin
		NV_DST24		UMETA(Hidden), // we don't need this				
		NV_INTZ			UMETA(Hidden), // they're mainly depth buffer formats
		NV_RAWZ			UMETA(Hidden), // who would want a depth buffer formatted texture?
		ATI_DST16		UMETA(Hidden),
		ATI_DST24		UMETA(Hidden),
		NV_NULL			UMETA(Hidden),
		ATI2N			UMETA(Hidden),
		ATI1N			UMETA(Hidden), // Vendor specific end
		COUNT     UMETA(Hidden),
		NONE = -1 UMETA(Hidden)
	};
}


UENUM(BlueprintType)
namespace EHammUErPropScaleAdjust
{
	enum Type
	{
		Ignore = 0,
		InPropUEr,
		UEScale
	};
}


UENUM(BlueprintType)
namespace EHammUErChannel
{
	enum Type
	{
		Red = 0,
		Green,
		Blue,
		Alpha
	};
}

/**
 *
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "HammUEr"))
class HAMMUER_API UHammUErDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

		UHammUErDeveloperSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
public:

	static const UHammUErDeveloperSettings* Get() { return GetDefault<UHammUErDeveloperSettings>(); }

#pragma region 
	/**
	*		Scale to use when importing. 100 cm is X original units.
	*		The smaller the value, the larger your map becomes.
	*		Try around 39.37 (100/2.54) for classic Source maps, for example.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Map Importing", meta = (DisplayName = "Scale conversion", ClampMin = "1.0", UIMin = "1.0"))
	float LocalConversionRate = 100.f;

	/**
	*		Amount to *multiply* original light values by.
	*		Unused for Quake .MAP import.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Map Importing", meta = (DisplayName = "Light intensity factor", ClampMin = "1.0", UIMin = "1.0"))
	int32 LightFactor = 100;

	/**
	*		Ignore: Ignore scaling when importing or placing props. 
	*		In PropUEr: Bakes the scale conversion into the UE version of the static mesh.
	*		UE scale: Keeps original X/Y/Z values, adjusts transform scale on VMF import.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Map Importing", meta = (DisplayName = "Prop scale handling"))
	TEnumAsByte<EHammUErPropScaleAdjust::Type> PropScaleType;

	/**
	*		If for example your ceiling has a nodraw side, but you still want it to stop light passing through, use this.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Map Importing", meta = (DisplayName = "NoDraw brushes have two-sided shadowing"))
	bool TwoSidedCull;


	/**
	*		Prepends every actor name with the imported level name.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Map Importing", meta = (DisplayName = "Prepend actors with level name"))
	bool AlwaysPrependFilename;


	/**
	*		When importing a map into a level, deletes all actors from the level that weren't reimported.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Map Importing", meta = (DisplayName = "Remove brush meshes that no longer exist on reimport"))
	bool ReimportCleanup;


	/**
	*		Logs verbose information to the output log. This has a performance impact.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Map Importing", meta = (DisplayName = "Verbose debug messages"))
	bool VerboseDebugEnabled;


	/**
	*		Applies the specified offset to the whole imported level.
	*		Note that offsets are applied *after* rotations,
	*		so if you import with 90 yaw, your X offset will become an Y offset.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Map Importing - Manipulation", meta = (DisplayName = "Use offset when importing level"))
	bool ImportNonZero;
	UPROPERTY(Config, EditAnywhere, Category = "Map Importing - Manipulation", meta = (DisplayName = "Offset to use when importing level"))
	FVector ImportOffset;


	/**
	*		Applies the specified rotation to the whole imported level.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Map Importing - Manipulation", meta = (DisplayName = "Use rotation when importing level"))
	bool ImportRotation;
	/**
	*		Amount to rotate imported meshes by.
	*		Note that this *may* break your map if you decoupled them from origin. 
	*		Shouldn't, but may.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Map Importing - Manipulation", meta = (DisplayName = "Rotation to use when importing level"))
	FVector ImportRotationValue;

	/**
	*		When using a Valve 220 format Quake 1 map, my example needed this, so...
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Map Importing - Manipulation", meta = (DisplayName = "Zero out Quake 1 rotation values"))
	bool IgnoreQ1Rotation;

	/**
	*		Can fix missing side / protrusion errors by removing as many chances of floating point errors as possible.
	*		This happens at an early stage in the process, and is the thermonuclear option.
	*		Try the other one first.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Map Importing - Error Correction", meta = (DisplayName = "Round brush points"))
	bool RoundPoints;

	/**
	*		Happens at a later stage than the first one.
	*		Much less aggressive and doesn't actually remove any possible sides, unlike the other one.
	*		Should be safe for all but the most tiny and intricate work.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Map Importing - Error Correction", meta = (DisplayName = "Round accepted points"))
	bool RoundMeshPoints;

	/**
	*		Rounds off displacement points to the nearest integer.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Map Importing - Error Correction", meta = (DisplayName = "Round displacements"))
	bool RoundDisplacementPoints;

	/**
	*		Can fix erroneous triangles appearing by being looser when discarding bad points.
	*		Setting this too high will also remove valid points, so be careful.
	*		Recommended value is in the 0.5-1.5 range
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Map Importing - Error Correction", meta = (DisplayName = "Error Correction Range"))
	float DiscardErrorRange = 1.0f;

	/**
	*		Can fix import errors. The lower this is set, the more options will be discarded as false positives.
	*		You *normally* shouldn't touch this, but lowering it will make it stricter,
	*		so if there's no minuscule detail in your map, you should be able to drop this to 100 safely if you're getting errors.
	*/
	UPROPERTY(Config, Editanywhere, Category = "Map Importing - Error Correction", meta = (DisplayName = "Intersect factor"))
	float IntersectRange = 10000.f;

	/**
	*		Nearness value used to merge vectors together when smoothing.
	*/
	UPROPERTY(Config, Editanywhere, Category = "Map Importing - Error Correction", meta = (DisplayName = "Vector weld margin"))
	float SmoothWeldRange = 0.1f;

	/**
	*		Recenters origin to inside the mesh for each separate mesh instead of building everything relative to 0,0,0.
	*/
	UPROPERTY(Config, Editanywhere, Category = "Map Importing - Brush Mesh Settings", meta = (DisplayName = "Decouple brush meshes from origin"))
	bool DecoupleMeshesFromZero;

	/**
	*		Do not use the new smoothing algorithm, but use the old flat shading for brush meshes and aggressive smoothing for displacements.
	*/
	UPROPERTY(Config, Editanywhere, Category = "Map Importing - Brush Mesh Settings", meta = (DisplayName = "Do not generate smoothing groups & normals"))
	bool DoNotUseSmoother;
	/// TODO: Should these values be that wide? Figure it out later.
	UPROPERTY(Config, Editanywhere, Category = "Map Importing - Brush Mesh Settings", meta = (DisplayName = "Maximum angle to generate smooth normals for", UIMin = "0.0", UIMax="360.0", ClampMin="0.0", ClampMax="360.0"))
	float SmoothingAngle = 60.f;

	/**
	*		Default lightmap size to use for generated brush meshes.
	*/
	UPROPERTY(Config, Editanywhere, Category = "Map Importing - Brush Mesh Settings", meta = (DisplayName = "Lightmap size", UIMin = "1.0", UIMax = "2048", ClampMin = "1.0"))
	int LightmapPrecision = 32;

	/**
	*		Channel you want to use in your blending material as alpha map to blend two textures.
	*/
	UPROPERTY(Config, Editanywhere, Category = "Map Importing - Brush Mesh Settings", meta = (DisplayName = "Default channel for displacement blending"))
	TEnumAsByte<EHammUErChannel::Type> DefaultDisplacementChannel = EHammUErChannel::Type::Alpha;

	/**
	*		Set brushes to 'generate overlap events'.
	*/
	UPROPERTY(Config, Editanywhere, Category = "Map Importing - Brush Mesh Settings", meta = (DisplayName = "Generate overlap events"))
	bool GenerateOverlaps;

	/**
	*		How many subdivisions to use for the quadratic patches in Q3 onwards.
	*/
	UPROPERTY(Config, Editanywhere, Category = "Map Importing - Brush Mesh Settings - idTech 3+", meta = (DisplayName = "Patch subdivision"))
	FIntPoint PatchSubDiv = FIntPoint(16);


	// TODO: think about how we want to handle game specific stuff -Luna
//UPROPERTY(Config, EditAnywhere, Category = "Processing")
//TSubclassOf<UHammUErProcessor> ProjectProcessor;
	UPROPERTY(Config, EditAnywhere, Category = "Map Importing - Default Classes")
	TSubclassOf<AStaticMeshActor> DefaultBrushActor;

	/**
	*		Generates temporary meshes for all unknown models the maps try to import.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Map Importing - Unknowns", meta = (DisplayName = "Generate placeholders for unknown models"))
	bool PlaceUnknownModels = true;

	/**
	*		Scale to use for the unknown model question marks. Too small? Increase the scale. Too big? decrease the scale.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Map Importing - Unknowns", meta = (DisplayName = "Scale", ClampMin = "1.0", UIMin = "1.0"))
	float ScaleUnknownModels = 5.0f;


#pragma endregion Map Importing

#pragma region
	/**
	*		Default texture dimensions to use when no material is found.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Texture & Material Importing", meta = (DisplayName = "Default texture size"))
	FIntPoint DefaultTextureSize = FIntPoint(512);

	/**
	*		Ignore the texture alpha.
	*		Turning this on can help, because most Source engine games seem to have a *lot* of bad alpha in textures.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Texture & Material Importing", meta = (DisplayName = "Don't use texture alpha on import"))
	bool NoTextureAlpha = false;

	/**
	*		Check this if you have less than 8 gigs of memory.
	*		It'll unload as many textures and materials as it can while importing to try and keep memory usage down.
	*		Note: When turned on, imported items will be 'invisible' until the next time you restart the engine.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Texture & Material Importing", meta = (DisplayName = "Try to save memory while importing textures and materials"))
	bool DoUnload = true;

	/**
	*		Colour that gets turned transparent when importing HL1 era textures.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Texture & Material Importing", meta = (DisplayName = "Masking colour when importing Half Life 1 era textures"))
	FColor HalfLifeAlphaColor = FColor(0, 0, 255, 255);

	/**
	*		When generating materials for unused textures, relax the rules.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Texture & Material Importing", meta = (DisplayName = "Relax ungenerated materials"))
	bool PermissiveMaterialCreation;

	/**
	*		Generates thumbnails for textures, materials and meshes.
	*		(Memory leak should have been fixed, as per UE-34672)
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Texture & Material Importing", meta = (DisplayName = "Generate thumbnails"))
	bool GenerateThumbs;

	/**
	*		If a texture ends with one of these (case-insensitive) tags, assume it's a normal texture.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Texture & Material Importing - Filtering", meta = (DisplayName = "Normal texture names end with"))
		TArray<FString> ImporterNormalExtensions = {"_n", "_normal"};

	/**
	*		Don't build materials for textures whose names end with any of these (case-insensitive) tags.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Texture & Material Importing - Filtering", meta = (DisplayName = "Don't build materials for textures ending with"))
	TArray<FString> ImporterIgnoreExtensions = {"_clr", "_ref"};

#pragma endregion Texture Importing


#pragma region
	/**
	*		Wanted format when exporting to VTF.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Texture & Material Exporting", meta = (DisplayName = "VTF texture format"))
	TEnumAsByte<EVTFImageFormat::Type> VTFTextureFormat;

	/**
	*		Filter out any textures that start with any of these (case-insensitive) tags.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Texture & Material Exporting - Filtering", meta = (DisplayName = "Filter out textures with names beginning with"))
	TArray<FString> TextureBeginsWith;
	/**
	*		Filter out any textures that contain any of these (case-insensitive) tags.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Texture & Material Exporting - Filtering", meta = (DisplayName = "Filter out textures with names containing"))
	TArray<FString> TextureContains;
	/**
	*		Filter out any textures that end with any of these (case-insensitive) tags.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Texture & Material Exporting - Filtering", meta = (DisplayName = "Filter out textures with names ending in"))
	TArray<FString> TextureEndsWith = {"_m", "_n"};

	/**
	*		You can use /<dirname>/ for example to narrow it down to a single directory, for example.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Texture & Material Exporting - Filtering", meta = (DisplayName = "Filter to *only* include materials containing"))
	TArray<FString> TextureKeepContains;

#pragma endregion Texture Exporting


#pragma region
	/**
	*		Recalculates vertex normals from face normals, discarding source normals.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "PropUEr", meta = (DisplayName = "Recalculate model normals"))
	bool SmoothModelNormals;
#pragma endregion PropUEr


#pragma region
	/**
	*		Do not use the rewritten brush mesh builder, but use the original code instead.
	**/
	UPROPERTY(Config, Editanywhere, Category = "Compatibility", meta = (DisplayName = "Use old brush mesh building mode"))
	bool UseOriginalBuildMode;

	/**
	*		Round UVs to 2 decimals. For reasons.
	**/
	UPROPERTY(Config, Editanywhere, Category = "Compatibility", meta = (DisplayName = "Use UV rounding"))
	bool UseUVRoundingMode;

	/**
	*		Try to optimise grouped brushes, instead of just combining all the solid triangles like before.
	*		Warning, this is *very* experimental and built uh, wrong, so it *will* crash if you try to use it for large groups, probably.
	**/
	UPROPERTY(Config, Editanywhere, Category = "Experimental", meta = (DisplayName = "Use new experimental group BSP clipping mode"))
	bool UseGroupBSP;

	/**
	*		Switches from the default fan mode to ear clipping in group BSP mode, to generate triangles in a different way, which should support concave polygons.
	**/
	UPROPERTY(Config, Editanywhere, Category = "Experimental", meta = (DisplayName = "Use new experimental ear clipping in group BSP clipping mode"))
	bool UseGroupEarClipping;

#pragma endregion Backwards Compatibility


	bool CheckIgnore;

	bool DebugCheckboxEnabled;

	int importTexturesMats;

	bool doNotGenerateMeshes;
	bool doNotGenerateEntities;
	bool doNotGenerateUnknownNotes;


	TArray<int32> reducedImportList;
	TArray<int32> reducedImportEntityList;

	FString previousDirectory;
	FString VTFExportDirectory;
	FString textureImportDirectory;
	FString textureSourceDirectory;
	FString modelImportDirectory;
	FString modelSourceDirectory;


	bool AddUserTag;
	FString WantedTag;

	
	bool discoverRebuild;

	bool generateVMFList;


	// entities



	bool ignorePitch;
	bool ignoreYaw;
	bool ignoreRoll;


	bool checkDegenerates;

	bool okayedTexturesLegal;
	bool okayedModelsLegal;

};
