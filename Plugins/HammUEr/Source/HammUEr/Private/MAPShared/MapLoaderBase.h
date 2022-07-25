// HammUEr
// Copyright 2015-2021 Turfster / NT Entertainment
// All Rights Reserved.


// This code is held together with kitten whiskers and toothpicks.
// There's nothing to see or learn here.
// Seriously.
// Turn back.

#pragma once
//#include "HammUErPrivatePCH.h"
#include "VMFTri.h"
#include "BaseEntity.h"
#include "MapArrayStructs.h"
#include "MAPSide.h"

enum class EMapType :int
{
	VMF,
	Valve220,
	Quake1,
	Quake3,
	Doom3,
	COD,
	Unknown = 255
};

class MapLoaderBase
{
public:
	MapLoaderBase()
	{}

	virtual ~MapLoaderBase()
	{}

	virtual void Open(std::string fileName)
	{}

	virtual bool PhaseOne()
	{
		return true;
	}

	virtual void PhaseTwo(std::vector<int> tws, std::vector<int> ths, std::vector<bool> tnd)
	{}

	int GetNumEntities();

	int GetEntityID(int index);

	MapEntityType GetEntityType(int index);
	BaseEntity* GetEntity(int index);
	std::string GetEntityName(int index);

	int GetNumBrushes();

	MapBrush* GetBrush(int index);

	std::vector<std::string> GetMaterials();

	void SetConversion(float value);

	void SetDontSmooth(bool value);

	void SetSmoothingAngle(float value);

	void SetSmoothingWeldRange(float value);

	std::string GetStageName();

	float GetStagePercentage();

	std::string importDirectory; // temporary

	std::map<std::string, std::string> GetWorldSpawnProperties();

	EMapType GetMapType();

protected:
	std::vector<std::string> materialNames;
	float convertUE;

	std::string stageName = "none";
	float stagePercentage = 0.f;

	// our data brushes, entities, etc
	std::vector<MapBrush*> brushes;
	std::vector<BaseEntity*> entities;

	bool doNotUseSmoother;
	float smoothWeldRange;
	float smoothingAngle;

	std::string mapType = "default";

	void CheckAdjacencies(int work, std::vector<VMFTri*>& inTriangles, std::vector<int>& visited, std::vector<MAPInt32Array>& adjacencies, std::vector<MAPInt32Array>& smoothingGroups);
	void SmoothTriangles(std::vector<VMFTri*>& trianglesToEdit, bool swapTwoThree);

	std::vector<int> textureWidths;
	std::vector<int> textureHeights;
	std::vector<bool> textureNoDraws;

	void AddOutputs(MAPNode* thisNode, BaseEntity* newLogic);

	MAPVector2D CalculateUVForSidePoint(MAPSide* sideToUse, MAPVector point, bool isQ3 = false, bool isDoom3 = false);

	std::map<std::string, std::string> worldInformation;

	EMapType MapType = EMapType::Unknown;
};

