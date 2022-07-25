// HammUEr
// Utility class
// Copyright 2015-2021 NT Entertainment

#pragma once
#include "Editor.h"
#include "MAPVector.h"

class MAPConverters
{
public:
		// Whoever decided to split FVector into FVector & FVector3f for UE5... prepare to die
		static FVector MAPtoF(MAPVector input)
		{
			return FVector(input.X, input.Y, input.Z);
		}

		static FVector3f MAPtoF3f(MAPVector input)
		{
			return FVector3f(input.X, input.Y, input.Z);
		}

		static FVector2D MAPtoF(MAPVector2D input)
		{
			return FVector2D(input.X, input.Y);
		}

		static FVector2f MAPtoF2f(MAPVector2D input)
		{
			return FVector2f(input.X, input.Y);
		}
		static FColor MAPtoF(MAPColor input)
		{
			return FColor(input.R, input.G, input.B, input.A);
		}

		static MAPVector FtoMAP(FVector input)
		{
			return MAPVector(input.X, input.Y, input.Z);
		}

		static MAPVector2D FtoMAP(FVector2D input)
		{
			return MAPVector2D(input.X, input.Y);
		}


};