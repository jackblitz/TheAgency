// HammUEr
// Copyright 2015-2021 Turfster / NT Entertainment
// All Rights Reserved.


// This code is held together with kitten whiskers and toothpicks.
// There's nothing to see or learn here.
// Seriously.
// Turn back.

#include "HammUErCommands.h"

#define LOCTEXT_NAMESPACE "FHammUErModule"

void FHammUErCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "HammUEr", "Bring up HammUEr window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
