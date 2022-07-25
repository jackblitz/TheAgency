// HammUEr
// Copyright 2015-2021 Turfster / NT Entertainment
// All Rights Reserved.


// This code is held together with kitten whiskers and toothpicks.
// There's nothing to see or learn here.
// Seriously.
// Turn back.

#include "TextureManipulation.h"
#include "ImageUtils.h"
#include "AssetRegistryModule.h"

#include "HammUErSettings.h"
#include "HammUErDeveloperSettings.h"

#include "Editor.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstance.h"
#include "ObjectTools.h"

#include "UObject/SavePackage.h"

UTexture2D* TextureManipulation::GetTextureForChain(UMaterial* inMaterial, EMaterialProperty inProperty)
{
	if (inMaterial != NULL)
	{
		TArray<UTexture*> chainTextures;
		TArray<UTexture*> parentTextures;
		TArray<FName> parameterNames;
		TArray<FName>parentparameterNames;
		UTexture2D* testTexture;

		if (inMaterial->GetTexturesInPropertyChain(inProperty, chainTextures, &parameterNames, NULL, ERHIFeatureLevel::Num, EMaterialQualityLevel::Num))
		{
			UMaterialInterface* myParent = ((UMaterialInstance*)inMaterial)->Parent;
			if (myParent != NULL)
			{

				// this is an instance
				myParent->GetTexturesInPropertyChain(inProperty, parentTextures, &parentparameterNames, NULL, ERHIFeatureLevel::Num, EMaterialQualityLevel::Num);
				for (int i = 0; i < parentTextures.Num(); i++)
					chainTextures.Remove(parentTextures[i]);
			}
			testTexture = NULL;

			// TODO: Seriously? Three times? 
			if (chainTextures.Num() > 0)
			{
				if (chainTextures.Num() == 1)
					testTexture = (UTexture2D*)chainTextures[0];
				else
				{
					for (int i = 0; i < chainTextures.Num(); i++)
					{
						if (chainTextures[i]->GetName().EndsWith("_D", ESearchCase::Type::IgnoreCase))
						{
							testTexture = (UTexture2D*)chainTextures[i];
							break;
						}
					}
					if (testTexture == NULL) // okay, so we've got nothing
					{
						for (int i = 0; i < chainTextures.Num(); i++)
						{
							bool filterOK = true;
							FString testTextureName = chainTextures[i]->GetName();
							for (int j = 0; j < UHammUErDeveloperSettings::Get()->TextureBeginsWith.Num(); j++)
							{
								if (testTextureName.StartsWith(UHammUErDeveloperSettings::Get()->TextureBeginsWith[j], ESearchCase::Type::IgnoreCase))
								{
									filterOK = false;
									break;
								}
							}
							if (!filterOK)
								break;
							for (int j = 0; j < UHammUErDeveloperSettings::Get()->TextureContains.Num(); j++)
							{
								if (testTextureName.Contains(UHammUErDeveloperSettings::Get()->TextureContains[j], ESearchCase::Type::IgnoreCase))
								{
									filterOK = false;
									break;
								}
							}
							if (!filterOK)
								break;
							for (int j = 0; j < UHammUErDeveloperSettings::Get()->TextureEndsWith.Num(); j++)
							{
								if (testTextureName.EndsWith(UHammUErDeveloperSettings::Get()->TextureEndsWith[j], ESearchCase::Type::IgnoreCase))
								{
									filterOK = false;
									break;
								}
							}
							if (filterOK)
							{
								testTexture = (UTexture2D*)chainTextures[i];
								break;
								// yeah, sure, whatever, it passed the test, let's go with this one
							}
						}
					}
					if (testTexture == NULL) // no texture to use?
					{
						return NULL;
					}
				}

				return testTexture;
			}

		}
		else
		{
			return NULL;
		}
	}
	return NULL;
}

void TextureManipulation::CreateTexture(int32 width, int32 height, TArray<FColor>& colorArray, FString ObjectName, FString importDirectory, bool hasAlpha)
{
	// last minute sanitizing, just in case we missed one
	ObjectName = ObjectName.Replace(TEXT("*"), TEXT("X"));
	ObjectName = ObjectName.Replace(TEXT("?"), TEXT("Q"));
	ObjectName = ObjectName.Replace(TEXT("!"), TEXT("I"));
	ObjectName = ObjectName.Replace(TEXT("."), TEXT("-"));
	ObjectName = ObjectName.Replace(TEXT("&"), TEXT("_"));
	ObjectName = ObjectName.Replace(TEXT(" "), TEXT("_"));


	FString NewPackageName = TEXT("/Game/") + (importDirectory == "" ? "Textures/" : importDirectory + "/Textures/") + ObjectName;
	UPackage* Package = CreatePackage(*NewPackageName);
	Package->FullyLoad();
	Package->Modify();

	FCreateTexture2DParameters FCT;
	FCT.bUseAlpha = hasAlpha;
	/*if (UHammUErDeveloperSettings::Get()->NoTextureAlpha)
		FCT.bUseAlpha = false;*/

	FString TextureName = ObjectName;
	if (TextureName.Contains("/"))
		TextureName = TextureName.Mid(TextureName.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd) + 1);

	UTexture2D* basetexture = FImageUtils::CreateTexture2D(width, height, colorArray, Package, *TextureName, RF_Public | RF_Standalone, FCT);
	FAssetRegistryModule::AssetCreated(basetexture);
	basetexture->PostEditChange();
	colorArray.Empty();

	FString PackageFileName = FPackageName::LongPackageNameToFilename(NewPackageName, FPackageName::GetAssetPackageExtension());
	try
	{

		if (UHammUErDeveloperSettings::Get()->GenerateThumbs)
			ThumbnailTools::GenerateThumbnailForObjectToSaveToDisk(basetexture);

		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.Error = GError;
		SaveArgs.bWarnOfLongFilename = false;
		SaveArgs.SaveFlags = SAVE_None;
							
		if (GEditor->SavePackage(Package, basetexture, *PackageFileName, SaveArgs))
		{
			Package->PostEditChange();
			FAssetRegistryModule::AssetCreated(basetexture);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("Could not save package %s"), *PackageFileName);
	}
	catch (...)
	{
		UE_LOG(LogTemp, Error, TEXT("Something went catastrophically wrong trying to save an unreal package for %s. In unreal code."), *ObjectName);
	}
}
