// HammUEr
// HammUEr Settings rework
// Copyright 2021 Turfster / NT Entertainment & Luna Ryuko
// All Rights Reserved.


#include "HammUErDeveloperSettings.h"

#include "Engine/StaticMeshActor.h"

UHammUErDeveloperSettings::UHammUErDeveloperSettings(const FObjectInitializer& ObjectInitializer)
	: UDeveloperSettings(ObjectInitializer)
{
	DefaultBrushActor = AStaticMeshActor::StaticClass();
	VTFTextureFormat = EVTFImageFormat::DXT5;

	LocalConversionRate = 100.f;
}