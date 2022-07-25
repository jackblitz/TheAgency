// HammUEr
// Copyright 2015-2021 Turfster / NT Entertainment
// All Rights Reserved.


// This code is held together with kitten whiskers and toothpicks.
// There's nothing to see or learn here.
// Seriously.
// Turn back.

#pragma once
#include "CoreMinimal.h"
#include "Engine/Texture2D.h"

class TextureManipulation
{
public:
	static UTexture2D* GetTextureForChain(UMaterial* inMaterial, EMaterialProperty inProperty);
	static void CreateTexture(int32 width, int32 height, TArray<FColor>& colorArray, FString ObjectName, FString importDirectory, bool hasAlpha);
};

void DecompressDXT1Image(int32 width, int32 height, uint8* data, TArray<FColor> &image);
void DecompressBlockDXT3(int32 x, int32 y, int32 stride, uint8* source, TArray<FColor> &image);
void DecompressBlockDXT5(int32 x, int32 y, int32 stride, uint8* source, TArray<FColor> &image);