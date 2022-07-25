// HammUEr
// Copyright 2015-2021 Turfster / NT Entertainment
// All Rights Reserved.


// This code is held together with kitten whiskers and toothpicks.
// There's nothing to see or learn here.
// Seriously.
// Turn back.

#pragma once
#include "VMFTri.h"
#include "VMFLogic.h"
#include <string>
#include <vector>
#include <map>
#include <algorithm>

class MapBrush
{

public:
	int id = -1;
	bool containsNoDraw = false;
	bool containsDisplacements = false;
	std::string brushName = "none";
	std::vector<VMFTri*> triangles;
};

class BaseEntity
{
public:
	int id;

	MAPVector origin;
	std::string classname;
	std::string name;

	MAPVector angles;

	std::vector<VMFOutput*> myOutputs;

	BaseEntity();
	~BaseEntity();

		std::map<std::string, std::string> GetOriginalData();
		void SetOriginalData(std::map<std::string, std::string> data);

	protected:
		std::map<std::string, std::string> originalData;
};

enum MapEntityType :int
{
	Unknown,
	FuncBox,
	Instance,
	FuncBrush,
	FuncStatic,
	LogicRelay,
	LogicAuto,
	PropStatic,
	PropDynamic,
	Path,
	Light,
	Spotlight,
	EnvironmentLight,
	Overlay
};

