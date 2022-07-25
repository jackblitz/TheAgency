// HammUEr
// Copyright 2015-2021 Turfster / NT Entertainment
// All Rights Reserved.


// This code is held together with kitten whiskers and toothpicks.
// There's nothing to see or learn here.
// Seriously.
// Turn back.

#pragma once
#include <string>

struct VMFOutput
{
public:
	std::string outputName;
	std::string targetEntity; // targetname or classname, accepts *
	std::string targetInput; // input of the target entity that will be triggered.
	std::string parameters; // data to pass
	int timeDelay; // number of seconds
	int fireTimes; // number of times to fire, -1 to always fire

	VMFOutput();

	static VMFOutput* CreateOutput(std::string input, std::string output);
};

