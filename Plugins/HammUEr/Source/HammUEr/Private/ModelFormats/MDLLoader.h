// HammUEr
// (c) 2015-2021 Turfster / NT Entertainment
// Source Model loader

// This code is FUGLY.

#pragma once
#include "../MAPShared/MAPVector.h"
#include "BaseModelLoader.h"

struct VVDHeader
{
	int id;
	int version;
	unsigned int checksum;
	int numLODs;
	int numLODVertices[8];
	int numFixups;
	int fixupTableStart;
	int vertexDataStart;
	int tangentDataStart;
};

class MDLReader:public BaseModelLoader
{
private:
	int id;
	int version;
	unsigned int checksum;
	int length;
	MAPVector globalScale;
	// skip 2 useless vectors
	MAPVector hullMin;
	MAPVector hullMax;
	// skip 3 more Vectors
	int flags;
	// skip 8 bytes

	int numTextures;
	int textureOffset;

	int numTextureSearchPaths;
	int textureSearchPathOffset;

	int numIncludeModels;
	int includeModelOffset;

	int numBodyParts;
	int bodyPartsOffset;

	uint8_t* DataPtr;
	uint8_t* head;

	// VTX File
	std::string VTXFileName;
	int VTXversion;
	int VTXvertCacheSize;
	unsigned short VTXmaxBonesPerStrip;
	unsigned short VTXmaxBonesPerTri;
	int VTXmaxBonesPerVert;
	unsigned int VTXchecksum; // must match!
	int VTXnumLODs; // must match
	int VTXmaterialReplacementListOffset;
	int VTXnumBodyParts;
	int VTXbodyPartOffset;

	uint8_t* VTXPtr;
	uint8_t* VTXhead;
	std::vector<uint8_t> VTXData;


	VVDHeader* VVDheader;
	uint8_t* VVDPtr;
	uint8_t* VVDhead;
	std::vector<uint8_t> VVDData;

	bool recalcNormals;
	std::string vertexListType;

	std::string ReadString(int offset);
	std::string ReadString(int offset, int maxLength);

	int ReadNextint();
	float ReadNextFloat();

	uint8_t ReadNextByte();

	std::vector<uint8_t> MDLData;
	std::string MDLFileName;
	int dataSize;

	std::vector<std::string> textureNames;
	std::vector<std::string> textureDirectories;
	std::vector<std::string> modelsToInclude;

	void GetTextureNames(std::vector<std::string>& theArray);
	void GetTextureDirectories(std::vector<std::string>& theArray);
	void GetModelsToInclude(std::vector<std::string>& theArray);

	void Load37();
	void Load44();
	void Load46();
	void Load49();
	void Load2531();

	bool GetVTX();

	bool GetVVD();

	bool IsMDL();
	bool IsSupported();

	void FillTriangles37();
	void FillTriangles44();
	void FillTriangles48();
	void FillTriangles49();
	void FillTriangles2531();

public:

	bool Open(std::string inFileName) override;

	void Import() override;

	void CleanUp() override;
};
