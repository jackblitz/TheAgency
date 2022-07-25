// HammUEr
// Generic settings catchall and databases
// Copyright 2015-2021 Turfster / NT Entertainment
// All Rights Reserved.


// This code is held together with kitten whiskers and toothpicks.
// There's nothing to see or learn here.
// Seriously.
// Turn back.

#pragma once
#include "HammUErPrivatePCH.h"

#include "PhysicsEngine/BodySetup.h"
#include "RawMesh.h"
#include "StaticMeshResources.h"
#include "MeshUtilities.h"
#include "Engine/StaticMeshActor.h"

class FModelListEntry :public TSharedFromThis<FModelListEntry>
{
public:
	FString name;
	FRotator fixRotation;
	bool doImport;

	FModelListEntry()
	{
		name = "";
		fixRotation = FRotator::ZeroRotator;
		doImport = false;
	}

	FModelListEntry(FString inName)
	{
		name = inName;
		fixRotation = FRotator::ZeroRotator;
		doImport = false;
	}

	FModelListEntry(FString inName, FRotator inRotation)
	{
		name = inName;
		fixRotation = inRotation;
		doImport = false;
	}
};

struct ModelImportInfo
{
public:
	FString typeName;
	float roll;
	float pitch;
	float yaw;

	ModelImportInfo()
	{
		typeName = "";
		roll = 0;
		pitch = 0;
		yaw = 0;
	}

	ModelImportInfo(FString inName)
	{
		typeName = inName;
		roll = 0;
		pitch = 0;
		yaw = 0;
	}

	ModelImportInfo(FString inName, FRotator inRotation)
	{
		typeName = inName;
		roll = inRotation.Roll;
		pitch = inRotation.Pitch;
		yaw = inRotation.Yaw;
	}
};

struct InstanceDatabaseEntry
{
public:
	FString instanceName;
	FString fileName;

	InstanceDatabaseEntry()
	{
		instanceName = "";
		fileName = "";
	}

	InstanceDatabaseEntry(FString iN, FString fN)
	{
		instanceName = iN;
		fileName = fN;
	}

	bool operator==(const InstanceDatabaseEntry &ide2) const
	{
		return ((instanceName == ide2.instanceName) && (fileName == ide2.fileName));
	}
};

class FMaterialListEntry :public TSharedFromThis<FMaterialListEntry>
{
public:
	FString oldName;
	int32 textureWidth;
	int32 textureHeight;
	bool ignore;
	int32 AssetDatabaseIndex;
	FString newMaterialName;
	UMaterialInterface* newMaterial;

	void SetTexWidth(const int32 newValue)
	{
		textureWidth = newValue;
	}

	void SetTexHeight(const int32 newValue)
	{
		textureHeight = newValue;
	}

	FMaterialListEntry()
	{
		ignore = false;
		textureWidth = 1;
		textureHeight = 1;
		oldName = "none";
		newMaterialName = "";
		AssetDatabaseIndex = -1;
		newMaterial = NULL;
	}

	FMaterialListEntry(TSharedPtr<FMaterialListEntry> original)
	{
		oldName = original->oldName;
		textureWidth = original->textureWidth;
		textureHeight = original->textureHeight;
		ignore = original->ignore;
		AssetDatabaseIndex = original->AssetDatabaseIndex;
		newMaterialName = original->newMaterialName;
		newMaterial = original->newMaterial;
	}
};

class HammUErSettings
{
public:
	static bool checkIgnore;

	static bool DebugCheckboxEnabled;  

	static bool DecoupleMeshesFromZero; 


	static int importTexturesMats; 

	static bool doNotGenerateMeshes; 
	static bool doNotGenerateEntities;
	static bool doNotGenerateUnknownNotes;



	static TArray<int32> reducedImportList;
	static TArray<int32> reducedImportEntityList;

	static FString previousDirectory;
	static FString VTFExportDirectory;
	static FString textureImportDirectory;
	static FString textureSourceDirectory;
	static FString modelImportDirectory;
	static FString modelSourceDirectory;

	static TArray<TSharedPtr<FMaterialListEntry>> masterMaterialList;

	static int32 FindInMaster(FString tofind);

	static const int32 currentConfigVersion = 4;

	static TArray<FAssetData> AssetData;
	static TArray<FAssetData> StaticMeshAssetData;
	static TArray<FAssetData> BPAssetData;   // Blueprints

	static bool permissiveMaterialCreation;

	static bool AddUserTag;	
	static FString WantedTag;

	HammUErSettings();

	static void SaveConfig();
	static void LoadConfig(bool reloadMasterList);
	static void GenerateDefaultSettings();

	static bool InstanceInDatabase(FString instance_name);
	static void LoadInstanceDatabase();
	static void SaveInstanceDatabase();
	static void AddInstance(FString instance_name, FString instance_filename);
	static FString GetInstance(FString instance_name);
	static TArray<InstanceDatabaseEntry> instanceDatabase;

	static UMaterial* MaterialInAssetDatabase(FString name);
	static int MaterialIndexAssetDatabase(FString input);

	static void GetMaterialSizes(UMaterial* inMaterial, int& width, int& height);
	
	static bool GetMaterialSizesForChain(UMaterial* inMaterial, int& width, int& height, EMaterialProperty inProperty = EMaterialProperty::MP_BaseColor);

	static FMaterialListEntry* GetMasterMaterialInfo(FString input);

	static FString BuildaStringFromList(TArray<FString> inputList);
	static TArray<FString> BuildArrayFromString(FString inputString);
	
	static UStaticMesh* CreateStaticMesh(struct FRawMesh& RawMesh, TArray<UMaterialInterface*>& Materials, UObject* InOuter, FName InName, bool noComplexCollision =false);


	static bool discoverRebuild;

	static bool generateVMFList;


	static void ReloadAssetDatabase();
	static void LoadStaticMeshDatabase();

	static UStaticMesh* FindInStaticMeshDatabase(FString name);

	// entities
	static int32 propScaleType;

	static bool smoothModelNormals;

	///Actually the opposite
	static bool useTextureAlpha;

	static void LoadModelSettings();
	static void SaveModelSettings();
	static TMap<FString, ModelImportInfo*> modelTypes;

	static FMatrix CreateRotationMatrix(float pitch, float roll, float yaw);
	static int FindInModelDatabase(FString modelname);
	static FRotator FindRotationInModelDatabase(FString modelname);

	static void LoadDecalSettings();
	static void SaveDecalSettings();
	static TMap<FString, float> decalScales;
	static float GetDecalScale(FString input);

	static void LoadBPDatabase();
	static UBlueprint* FindInBPDatabase(FString BPname);

	static bool ignorePitch;
	static bool ignoreYaw;
	static bool ignoreRoll;

	static bool ignoreQ1Rotation;

	static bool checkDegenerates;

	static bool okayedTexturesLegal;
	static bool okayedModelsLegal;


	static FString defaultImportHeader;
	static TArray<FString> toDeleteStaticMeshActorList;
	static TMap<FString, AStaticMeshActor*> existingStaticMeshActorMap;


};