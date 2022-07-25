// HammUEr
// Generic settings catchall and database lookups
// Copyright 2015-2021 Turfster / NT Entertainment
// All Rights Reserved.


// This code is held together with kitten whiskers and toothpicks.
// There's nothing to see or learn here.
// Seriously.
// Turn back.

#include "HammUErSettings.h"
#include "Modules/ModuleManager.h"
#include "DesktopPlatformModule.h"
#include "AssetRegistryModule.h"

#include "../Private/GeomFitUtils.h"
#include "Runtime/Launch/Resources/Version.h" 
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/HAL/PlatformFileManager.h"
#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "Runtime/Engine/Classes/Materials/Material.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstance.h"
#include "Misc/ScopedSlowTask.h"

#include "HammUErDeveloperSettings.h"

#if ENGINE_MAJOR_VERSION < 5
#else
#include "Engine/StaticMeshSourceData.h"
#endif

bool HammUErSettings::checkIgnore;

bool HammUErSettings::DebugCheckboxEnabled;

bool HammUErSettings::checkDegenerates;

bool HammUErSettings::okayedTexturesLegal;
bool HammUErSettings::okayedModelsLegal;

int HammUErSettings::importTexturesMats;

bool HammUErSettings::doNotGenerateMeshes;
bool HammUErSettings::doNotGenerateEntities;
bool HammUErSettings::doNotGenerateUnknownNotes;


TArray<int32> HammUErSettings::reducedImportList;
TArray<int32> HammUErSettings::reducedImportEntityList;
TArray<TSharedPtr<FMaterialListEntry>> HammUErSettings::masterMaterialList;

FString HammUErSettings::previousDirectory;

TArray<FAssetData> HammUErSettings::AssetData;
TArray<FAssetData> HammUErSettings::StaticMeshAssetData;
TArray<FAssetData> HammUErSettings::BPAssetData;
TArray<InstanceDatabaseEntry> HammUErSettings::instanceDatabase;

FString HammUErSettings::textureImportDirectory;
FString HammUErSettings::textureSourceDirectory;
FString HammUErSettings::modelImportDirectory;
FString HammUErSettings::modelSourceDirectory;

FString HammUErSettings::VTFExportDirectory;

bool HammUErSettings::discoverRebuild;

bool HammUErSettings::generateVMFList;

TMap<FString, ModelImportInfo*> HammUErSettings::modelTypes;
TMap<FString, float> HammUErSettings::decalScales;

bool HammUErSettings::ignorePitch;
bool HammUErSettings::ignoreRoll;
bool HammUErSettings::ignoreYaw;


TArray<FString> HammUErSettings::toDeleteStaticMeshActorList;
TMap<FString, AStaticMeshActor*> HammUErSettings::existingStaticMeshActorMap = {};

FString HammUErSettings::defaultImportHeader;
bool HammUErSettings::AddUserTag = true;
FString HammUErSettings::WantedTag = "HammUEr_Brush";

HammUErSettings::HammUErSettings()
{
	DebugCheckboxEnabled = false;
	reducedImportList.Empty();
	VTFExportDirectory = "";
	previousDirectory = "";

	generateVMFList = false;
	defaultImportHeader = "";

	AddUserTag = false;
	WantedTag = "HammUEr_Brush";	// make this user editable down the line in the UI
}

FString HammUErSettings::BuildaStringFromList(TArray<FString> inputList)
{
	FString result = "";
	for (int i = 0; i < inputList.Num(); i++)
		result += (inputList[i]) + (i == inputList.Num() - 1 ? "" : ",");
	return result;
}

TArray<FString> HammUErSettings::BuildArrayFromString(FString inputString)
{
	TArray<FString> result;
	FString temp;
	int32 pos;
	inputString.TrimStartInline();
	inputString.TrimEndInline();
	result.Empty();
	while (inputString.Len() > 0)
	{
		pos = inputString.Find(",");
		if (pos == -1)
			pos = inputString.Len();
		temp = inputString.Mid(0, pos);
		temp.TrimStartInline();
		temp.TrimEndInline();
		result.Add(temp);
		inputString.RemoveAt(0, pos + 1);
	}
	return result;
}

void HammUErSettings::SaveModelSettings()
{
	FString configName = FPaths::ProjectConfigDir() + "hammUErModels.cfg";
	FString SaveText="";
	for (const auto& modelInfo : modelTypes)
	{
		modelInfo.Value->typeName = modelInfo.Value->typeName.Replace(TEXT("\\"), TEXT("/"));
		SaveText += modelInfo.Key + "," + modelInfo.Value->typeName +","+FString::SanitizeFloat(modelInfo.Value->roll)+","+ FString::SanitizeFloat(modelInfo.Value->pitch)+","+ FString::SanitizeFloat(modelInfo.Value->yaw) + "\r\n";
	}
	FFileHelper::SaveStringToFile(SaveText, *configName);
}

// TODO: Move out to a helper class
FMatrix HammUErSettings::CreateRotationMatrix(float theta, float psi, float phi) // pitch, roll, yaw
{
	FMatrix rotationMatrix = FMatrix::Identity;
	theta = theta * PI / 180.f;
	psi = psi * PI / 180.f;
	phi = phi * PI / 180.f;
	rotationMatrix.SetAxis(0, FVector(FMath::Cos(theta)*FMath::Cos(phi),
		FMath::Cos(theta)*FMath::Sin(phi), 
		-FMath::Sin(theta)));
	rotationMatrix.SetAxis(1, FVector(FMath::Sin(psi)*FMath::Sin(theta)*FMath::Cos(phi) - FMath::Cos(psi)*FMath::Sin(phi),
		FMath::Sin(psi)*FMath::Sin(theta)*FMath::Sin(phi) + FMath::Cos(psi)*FMath::Cos(phi),
		FMath::Cos(theta)*FMath::Sin(psi)));
	rotationMatrix.SetAxis(2, FVector(FMath::Cos(psi)*FMath::Sin(theta)*FMath::Cos(phi) + FMath::Sin(psi)*FMath::Sin(phi),
		FMath::Cos(psi)*FMath::Sin(theta)*FMath::Sin(phi) - FMath::Sin(psi)*FMath::Cos(phi),
		FMath::Cos(theta)*FMath::Cos(psi)));
	return rotationMatrix;
}

void HammUErSettings::LoadDecalSettings()
{
	if (decalScales.Num() == 0)
	{
		FString configName = FPaths::ProjectConfigDir() + "hammUErDecals.cfg";
		if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*(configName)))
		{
			TArray<FString> configLines;
			FFileHelper::LoadANSITextFileToStrings(*(configName), NULL, configLines);
			for (int i = 0; i < configLines.Num(); i++)
			{
				if (configLines[i] != "")
				{
					TArray<FString> brokenUp;
					int counter = 0;
					configLines[i].ParseIntoArray(brokenUp, TEXT(","), false);
					if (brokenUp.Num() == 2)
					{
						float value = FCString::Atof(*brokenUp[1]);
						decalScales.Add(brokenUp[0], value);
					}
				}
			}
			configLines.Empty();
		}
	}
}

void HammUErSettings::SaveDecalSettings()
{
	FString configName = FPaths::ProjectConfigDir() + "hammUErDecals.cfg";
	FString SaveText = "";
	for (const auto& decalInfo : decalScales)
		SaveText += decalInfo.Key + "," + FString::SanitizeFloat(decalInfo.Value) + "\r\n";
	FFileHelper::SaveStringToFile(SaveText, *configName);
}

float HammUErSettings::GetDecalScale(FString input)
{
	if (decalScales.Num() == 0)
		LoadDecalSettings();
	if (decalScales.Num() > 0)
	{
		if (decalScales.Contains(input.ToUpper()))
		{
			return decalScales[input.ToUpper()];
		}
		else
		{
			FString input2 = input.ToUpper();
			TArray<FString> keyList;
			decalScales.GetKeys(keyList);
			for (int i = 0; i < decalScales.Num(); i++)
			{
				if (keyList[i].Contains(input2))
					return decalScales[keyList[i]];
			}
		}
	}
	return 1.f;
}

void HammUErSettings::LoadModelSettings()
{
	if (modelTypes.Num() == 0)
	{
		FString configName = FPaths::ProjectConfigDir() + "hammUErModels.cfg";
		if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*(configName)))
		{
			TArray<FString> configLines;
			FFileHelper::LoadANSITextFileToStrings(*(configName), NULL, configLines);
			for (int i = 0; i < configLines.Num(); i++)
			{
				if (configLines[i] != "")
				{
					TArray<FString> brokenUp;
					int counter = 0;
					configLines[i].ParseIntoArray(brokenUp, TEXT(","), false);
					if (brokenUp.Num() == 2)
					{
						ModelImportInfo* mii = new ModelImportInfo(brokenUp[1]);
						modelTypes.Add(brokenUp[0], mii);
					}
					else
						if (brokenUp.Num() == 5)
						{
							ModelImportInfo* mii = new ModelImportInfo(brokenUp[1]);
							mii->roll = FCString::Atof(*brokenUp[2]);
							mii->pitch = FCString::Atof(*brokenUp[3]);
							mii->yaw = FCString::Atof(*brokenUp[4]);
							modelTypes.Add(brokenUp[0], mii);
						}
				}
			}
			configLines.Empty();
		}
	}
}

int HammUErSettings::FindInModelDatabase(FString modelname)
{
	FString workName = modelname;

	if (modelTypes.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Model database empty?"));
		LoadModelSettings();
	}
	if (modelTypes.Num() > 0)
	{
		if (modelTypes.Contains(modelname))
		{
			UE_LOG(LogTemp, Log, TEXT("Found %s"), *modelname);
			FString data = modelTypes[modelname]->typeName;
			if (data == "V0")
				return 1;
			if (data == "V1")
				return 2;
			else 
				return 0;
		}
		else
		{
			TArray<FString> keys;
			modelTypes.GenerateKeyArray(keys);
			int index = -1;
			for (int i = 0; i < keys.Num(); i++)
			{
				if (modelname.Contains(keys[i]))
				{
					index = i;
					break;
				}
			}
			if (index > -1)
			{
				UE_LOG(LogTemp, Log, TEXT("Tried to find %s, using %s"), *modelname, *keys[index]);

				FString data = modelTypes[keys[index]]->typeName;
				if (data == "V0")
					return 1;
				if (data == "V1")
					return 2;
				else
					return 0;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Could not find %s"), *modelname);
				return 0;

			}
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Could not find %s, final"), *modelname);
	return 0;
}


FRotator HammUErSettings::FindRotationInModelDatabase(FString modelname)
{
	FString workName = modelname;

	if (modelTypes.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Model database wasn't loaded. Why."));
		LoadModelSettings();
	}
	if (modelTypes.Num() > 0)
	{
		if (modelTypes.Contains(modelname))
		{
			UE_LOG(LogTemp, Log, TEXT("Found %s"), *modelname);
			return FRotator(modelTypes[modelname]->pitch, modelTypes[modelname]->yaw, modelTypes[modelname]->roll);
		}
		else
		{
			TArray<FString> keys;
			modelTypes.GenerateKeyArray(keys);
			int index = -1;
			for (int i = 0; i < keys.Num(); i++)
			{
				if (modelname.Contains(keys[i]))
				{
					index = i;
					break;
				}
			}
			if (index > -1)
			{
				UE_LOG(LogTemp, Log, TEXT("Tried to find %s, using %s"), *modelname, *keys[index]);

				return FRotator(modelTypes[keys[index]]->pitch, modelTypes[keys[index]]->yaw, modelTypes[keys[index]]->roll);
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("No rotation database information found for %s"), *modelname);
				return FRotator::ZeroRotator;

			}
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Could not find %s"), *modelname);
	return FRotator::ZeroRotator;
}

void HammUErSettings::SaveConfig()
{
	FString SaveText = "";

	FString configName = FPaths::ProjectConfigDir() + "hammUErPlugin.cfg";
	SaveText = "";
		SaveText += "hammUErDirectory," + previousDirectory + "\r\n";
	SaveText += "hammUErDebugMaterials," + FString::FromInt((int32)HammUErSettings::DebugCheckboxEnabled) + "\r\n";
	SaveText += "hammUErWriteMatList," + FString::FromInt((int32)HammUErSettings::generateVMFList) + "\r\n";
	SaveText += "hammUErVTFExportDir," + VTFExportDirectory + "\r\n";
	SaveText += "hammUErImportTextureDirectory," + textureImportDirectory + "\r\n";
	SaveText += "hammUErSourceTextureDirectory," + textureSourceDirectory + "\r\n";
	SaveText += "hammUErImportModelDirectory," + modelImportDirectory + "\r\n";
	SaveText += "hammUErSourceModelDirectory," + modelSourceDirectory + "\r\n";
	SaveText += "hammUErDefaultImportHeader," + HammUErSettings::defaultImportHeader + "\r\n";
	SaveText += "hammUErNoNotes," + FString::FromInt(HammUErSettings::doNotGenerateUnknownNotes) + "\r\n";
	if (HammUErSettings::okayedModelsLegal)
		SaveText+="hammUErModelsLegal,"+ FString::FromInt(HammUErSettings::okayedModelsLegal) + "\r\n";
	if (HammUErSettings::okayedTexturesLegal)
		SaveText += "hammUErTexturesLegal," + FString::FromInt(HammUErSettings::okayedTexturesLegal) + "\r\n";
	SaveText += "hammUErAddUserTag," + FString::FromInt((int32)HammUErSettings::AddUserTag) + "\r\n";
	SaveText += "hammUErWantedTag," + HammUErSettings::WantedTag + "\r\n";
	SaveText += "hammUErVersion," + FString::FromInt(currentConfigVersion) + "\r\n";

	if (HammUErSettings::masterMaterialList.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Master material list is empty"));
	}
	for (int i = 0; i < HammUErSettings::masterMaterialList.Num(); i++)
	{
		SaveText += HammUErSettings::masterMaterialList[i]->oldName + "," + FString::FromInt(HammUErSettings::masterMaterialList[i]->textureWidth) + "," + FString::FromInt(HammUErSettings::masterMaterialList[i]->textureHeight) + "," + FString::FromInt((int)HammUErSettings::masterMaterialList[i]->ignore) + ",";
		if (HammUErSettings::masterMaterialList[i]->newMaterial != NULL)
		{
			FString MatDir = HammUErSettings::masterMaterialList[i]->newMaterial->GetPathName();
			FString matString = HammUErSettings::masterMaterialList[i]->newMaterial->GetName();
			MatDir.RemoveFromEnd(matString);
			MatDir.RemoveFromEnd(".");
			int firstslash = MatDir.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
			int secondslash = MatDir.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd, firstslash - 1);
			FString MatDirectoryName = (MatDir.Mid(secondslash + 1, (firstslash - 1) - (secondslash))).ToUpper();
			if (MatDirectoryName.ToUpper() != "GAME")
				matString = MatDirectoryName + "/" + matString;

			SaveText += matString;
		}
		else
		{
			FString MatDir = HammUErSettings::masterMaterialList[i]->newMaterialName;
			if (MatDir != "")
			{
				int offset = MatDir.Find(".", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
				if (offset == -1)
					offset = MatDir.Len();			// FIX HERE 16112016
				MatDir = MatDir.Mid(0, offset);
				FString newName = MatDir;
				newName = MatDir.Mid(MatDir.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd) + 1);
				int firstslash = MatDir.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
				int secondslash = MatDir.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd, firstslash - 1);
				FString MatDirectoryName = (MatDir.Mid(secondslash + 1, (firstslash - 1) - (secondslash))).ToUpper();
				if (MatDirectoryName.ToUpper() != "GAME")
					newName = MatDirectoryName + "/" + newName;
				if (newName == "/")
					newName = "";
				SaveText += newName;
			}
		}
		SaveText += "\r\n";
	}
	FFileHelper::SaveStringToFile(SaveText, *configName);

}

void HammUErSettings::GenerateDefaultSettings()
{

	DebugCheckboxEnabled = true;
	generateVMFList = false;

	textureImportDirectory = "";

	WantedTag = "HammUEr_Brush";	// TODO: Make this user editable in the UI, and the AddTag toggle as well
}


// TODO 2021-05-27 God damn this is fucking shameful, don't let me hack stuff late at night
void HammUErSettings::LoadConfig(bool reloadMasterList)
{
	FString configName = FPaths::ProjectConfigDir() + "hammUErPluginShared.cfg";
	if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*(configName)))
	{
		TArray<FString> configLines;
		FFileHelper::LoadANSITextFileToStrings(*(configName), NULL, configLines);

		if (configLines.Num() < 10) // something's gone wrong
		{
			UE_LOG(LogTemp, Warning, TEXT("Error loading config file. Retrying with (hopefully) UTF-16 support."));
			FString UTF16test;
			FFileHelper::LoadFileToString(UTF16test, *configName);
			UTF16test = UTF16test.Replace(TEXT("\r"), TEXT(""));
			UTF16test.ParseIntoArray(configLines, TEXT("\n"), true);
			UTF16test.Empty();
			if (configLines.Num() < 10) // okay, something is definitely wrong
			{
				UE_LOG(LogTemp, Error, TEXT("Critical error loading config file. Loading default config settings."));
				GenerateDefaultSettings();
				return;
			}
		}

		int32 commaposition;
		FString parameter;
		FString value;

		int32 configVersion = 0;

		for (int i = 0; i < configLines.Num(); i++)
		{
			if (configLines[i] == "")
				continue;
			commaposition = configLines[i].Find(TEXT(","), ESearchCase::IgnoreCase, ESearchDir::FromStart, 0);
			parameter = configLines[i].Mid(0, commaposition);
			configLines[i].RemoveAt(0, commaposition + 1);
			configLines[i].TrimStartInline();
			value = configLines[i];
			if (parameter.StartsWith("hammer", ESearchCase::CaseSensitive)) // update older version config file
				parameter = parameter.Replace(TEXT("hammer"), TEXT("hammUEr"));
						
							if (parameter == "hammUErIgnoreTaggedActors")
							{
								HammUErSettings::checkIgnore = FCString::Atoi(*value) == 1 ? true : false;
							}
							else
										if (parameter == "hammUErNoNotes")
										{
											if (!reloadMasterList)
												HammUErSettings::doNotGenerateUnknownNotes = FCString::Atoi(*value) == 1 ? true : false;
										}
										else
											if (parameter == "hammUErModelsLegal")
											{
												if (!reloadMasterList)
													HammUErSettings::okayedModelsLegal = FCString::Atoi(*value) == 1 ? true : false;
											}
											else
												if (parameter == "hammUErTexturesLegal")
												{
													if (!reloadMasterList)
														HammUErSettings::okayedTexturesLegal = FCString::Atoi(*value) == 1 ? true : false;
												}
												else
																			if (parameter == "hammUErDefaultImportHeader")
																			{
																				if (!reloadMasterList)
																					HammUErSettings::defaultImportHeader = value;
																			}
																			else
																			
																										
																												if (parameter == "hammUErWriteMatList")
																												{
																													if (!reloadMasterList)
																														HammUErSettings::generateVMFList = FCString::Atoi(*value) == 1 ? true : false;
																												}
																												else
																													
																														
																																if (parameter == "hammUErDebugMaterials")
																																{
																																	if (!reloadMasterList)
																																		HammUErSettings::DebugCheckboxEnabled = FCString::Atoi(*value) == 1 ? true : false;
																																}
																																else
																																																				
																																															
																																															
																																																					if (parameter == "hammUErWantedTag")
																																																					{
																																																						if (value.IsEmpty())
																																																							value = "HammUEr_Brush";

																																																						if (!reloadMasterList)
																																																							HammUErSettings::WantedTag = value;
																																																					}		
																																																					else
																																																					if (parameter == "hammUErAddUserTag")
																																																					{
																																																						if (!reloadMasterList)
																																																							HammUErSettings::AddUserTag = FCString::Atoi(*value) == 1 ? true : false;
																																																					}		
																																																					else																																																					
																																																						if (parameter == "hammUErVersion")
																																																						{
																																																							configVersion = FCString::Atoi(*value);
																																																							if (!reloadMasterList)
																																																								break;
																																																						}
		}
	}

	configName =  FPaths::ProjectConfigDir() + "hammUErPlugin.cfg";
	if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled) UE_LOG(LogTemp, Log, TEXT("Reading configuration from %s"), *configName);
	if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*(configName)))
	{
		TArray<FString> configLines;
		FFileHelper::LoadANSITextFileToStrings(*(configName), NULL, configLines);

		if (configLines.Num() < 10) // something's gone wrong
		{
			UE_LOG(LogTemp, Warning, TEXT("Error loading config file. Retrying with (hopefully) UTF-16 support."));
			FString UTF16test;
			FFileHelper::LoadFileToString(UTF16test, *configName);
			UTF16test = UTF16test.Replace(TEXT("\r"), TEXT(""));
			UTF16test.ParseIntoArray(configLines, TEXT("\n"), true);
			UTF16test.Empty();
			if (configLines.Num() < 10) // okay, something is definitely wrong
			{
				UE_LOG(LogTemp, Error, TEXT("Critical error loading config file. Loading default config settings."));
				GenerateDefaultSettings();
				return;
			}
		}
		int32 commaposition;
		FString parameter;
		FString value;

		int32 configVersion = 0;

		TSharedPtr<FMaterialListEntry> newEntry;

		int32 texWidth, texHeight;

		HammUErSettings::masterMaterialList.Empty();

		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		AssetData.Empty();
		AssetRegistryModule.Get().GetAssetsByClass("Material", AssetData, true);
		TArray<FAssetData> AssetData2;
		AssetRegistryModule.Get().GetAssetsByClass("MaterialInstance", AssetData2, true);
		AssetData.Append(AssetData2);

		FScopedSlowTask SlowTask(configLines.Num(), FText::FromString("(Re-)Loading master material list from config file..."));
		SlowTask.MakeDialog();

		// TODO: delete
	bool loadedSVNVersion = false;
		for (int i = 0; i < configLines.Num(); i++)
		{
			SlowTask.EnterProgressFrame(1);
			if (configLines[i] == "")
				continue;
			commaposition = configLines[i].Find(TEXT(","), ESearchCase::IgnoreCase, ESearchDir::FromStart, 0);
			parameter = configLines[i].Mid(0, commaposition);
			configLines[i].RemoveAt(0, commaposition + 1);
			configLines[i].TrimStartInline();
			value = configLines[i];
			if (parameter.StartsWith("hammer", ESearchCase::CaseSensitive)) // update older version config file
				parameter = parameter.Replace(TEXT("hammer"), TEXT("hammUEr"));
		
							if (parameter == "hammUErNoNotes")
							{
								if (!reloadMasterList && !loadedSVNVersion)
									HammUErSettings::doNotGenerateUnknownNotes = FCString::Atoi(*value) == 1 ? true : false;
							}
							else
							if (parameter == "hammUErModelsLegal")
						{
							if (!reloadMasterList && !loadedSVNVersion)
								HammUErSettings::okayedModelsLegal = FCString::Atoi(*value) == 1? true:false;
						}
						else
							if (parameter == "hammUErTexturesLegal")
							{
								if (!reloadMasterList && !loadedSVNVersion)
									HammUErSettings::okayedTexturesLegal = FCString::Atoi(*value) == 1 ? true : false;
							}
							else
							if (parameter == "hammUErDefaultImportHeader")
							{
								if (!reloadMasterList && !loadedSVNVersion)
									HammUErSettings::defaultImportHeader = value;
							}
						else
							if (parameter == "hammUErDirectory")
							{
								if (!reloadMasterList)
									HammUErSettings::previousDirectory = value;
							}
							else
								if (parameter == "hammUErImportModelDirectory")
								{
									if (!reloadMasterList)
										HammUErSettings::modelImportDirectory = value;
								}
								else
									if (parameter == "hammUErSourceModelDirectory")
									{
										if (!reloadMasterList)
											HammUErSettings::modelSourceDirectory = value;
									}
									else

											if (parameter == "hammUErImportTextureDirectory")
											{
												if (!reloadMasterList)
													HammUErSettings::textureImportDirectory = value;
											}
											else
												if (parameter == "hammUErSourceTextureDirectory")
												{
													if (!reloadMasterList)
														HammUErSettings::textureSourceDirectory = value;
												}
												else
														if (parameter == "hammUErWriteMatList")
														{
															if (!reloadMasterList && !loadedSVNVersion)
																HammUErSettings::generateVMFList = FCString::Atoi(*value) == 1 ? true : false;
														}
														else

																	
																		if (parameter == "hammUErDebugMaterials")
																		{
																			if (!reloadMasterList && !loadedSVNVersion)
																				HammUErSettings::DebugCheckboxEnabled = FCString::Atoi(*value) == 1 ? true : false;
																		}
																		else
																																
																																
																																						if (parameter == "hammUErVTFExportDir")
																																						{
																																							if (!reloadMasterList)
																																								HammUErSettings::VTFExportDirectory = value;
																																						}
																																						else
																																							if (parameter == "hammUErVersion")
																																							{
																																								configVersion = FCString::Atoi(*value);
																																								if (!reloadMasterList)
																																									break;
																																							}
																																							else
																																								if (parameter == "hammUErWantedTag")
																																								{
																																									if (value.IsEmpty())
																																										value = "HammUEr_Brush";

																																									HammUErSettings::WantedTag = value;
																																									if (!reloadMasterList)
																																										break;
																																								}	
																																								else
																																									if (parameter == "hammUErAddUserTag")
																																									{
																																										if (!reloadMasterList)
																																											HammUErSettings::AddUserTag = FCString::Atoi(*value) == 1 ? true : false;
																																									}		
																													else
																													{
																														if (configVersion == HammUErSettings::currentConfigVersion)
																														{
																															commaposition = value.Find(TEXT(","), ESearchCase::IgnoreCase, ESearchDir::FromStart, 0);
																															texWidth = FCString::Atoi(*value.Mid(0, commaposition));
																															value.RemoveAt(0, commaposition + 1);
																															commaposition = value.Find(TEXT(","), ESearchCase::IgnoreCase, ESearchDir::FromStart, 0);
																															texHeight = FCString::Atoi(*value.Mid(0, commaposition));
																															value.RemoveAt(0, commaposition + 1);
																															commaposition = value.Find(TEXT(","), ESearchCase::IgnoreCase, ESearchDir::FromStart, 0);
																															bool bIgnore = (FCString::Atoi(*value.Mid(0, commaposition)) == 1);
																															value.RemoveAt(0, commaposition + 1);
																															value.TrimEndInline();
																															if (value == "/")
																																value = "";

																															// do material list here
																															newEntry = MakeShareable(new FMaterialListEntry());
																															newEntry->oldName = parameter.ToUpper();
																															newEntry->textureHeight = texHeight;
																															newEntry->textureWidth = texWidth;
																															if (parameter.Contains("nodraw") || parameter.ToLower() == "clip" || parameter.ToLower() == "origin" || parameter.Contains("toolsorigin") || parameter.Contains("toolsclip"))
																																bIgnore = true;
																															newEntry->ignore = bIgnore;

																															FString newmatdir = "";
																															if (value.Contains("/"))
																															{
																																FString temp = value;
																																int32 slashpos = value.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
																																value = value.Mid(slashpos + 1);
																																temp.RemoveAt(slashpos, temp.Len() - slashpos);
																																if (temp.Contains("/"))
																																{
																																	slashpos = value.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
																																	newmatdir = temp.Mid(slashpos + 1);
																																}
																																else
																																	newmatdir = temp;
																															}

																															int index = -1;
																															if (value == "")
																															{
																																newEntry->newMaterialName = "";
																																newEntry->newMaterial = NULL;
																															}
																															else
																															{
																																for (int j = 0; j < AssetData.Num(); j++)
																																{
																																	if (AssetData[j].AssetName.ToString() == value)
																																	{
																																		if (newmatdir != "")
																																		{
																																			FString testdir = AssetData[j].PackagePath.ToString();
																																			if (AssetData[j].PackagePath.ToString().Contains(newmatdir))
																																			{
																																				index = j;
																																				break;
																																			}
																																		}
																																		else
																																		{
																																			index = j;
																																			break;
																																		}
																																	}
																																}
																																if (index == -1)
																																{
																																	if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
																																		UE_LOG(LogTemp, Warning, TEXT("Could not find exact match for %s, looking for approximate matches"), *(newmatdir + "/" + value));
																																	for (int j = 0; j < AssetData.Num(); j++)
																																	{
																																		if (AssetData[j].AssetName.ToString().Contains(value))
																																		{
																																			if (newmatdir != "")
																																			{
																																				FString testdir = AssetData[j].PackagePath.ToString();
																																				if (AssetData[j].PackagePath.ToString().Contains(newmatdir))
																																				{
																																					index = j;
																																					break;
																																				}
																																			}
																																			else
																																			{
																																				index = j;
																																				break;
																																			}
																																		}
																																	}
																																}
																																/*if (index > -1)
																																	newEntry->newMaterial = (UMaterial*)AssetData[index].GetAsset();
																																else
																																	newEntry->newMaterial = NULL;*/
																																if (index > -1)
																																{
																																	newEntry->AssetDatabaseIndex = index;
																																	newEntry->newMaterialName = (AssetData[index].ObjectPath).ToString();
																																	//newEntry->newMaterial = (UMaterialInstance*)LoadObject<UMaterial>(NULL, *newEntry->newMaterialName, NULL, LOAD_None, NULL);
																																}
																																else
																																{
																																	UE_LOG(LogTemp, Warning, TEXT("Could not find %s in Asset Database?"), *value);
																																	newEntry->AssetDatabaseIndex = -1;
																																	newEntry->newMaterialName = "";
																																}
																															}

																															HammUErSettings::masterMaterialList.AddUnique(newEntry);
																														}
																													}
		}

		
	
	}
	else
	{
		if (!reloadMasterList)
		{
			GenerateDefaultSettings();
		}
	}
}

void HammUErSettings::GetMaterialSizes(UMaterial* inMaterial, int& width, int& height)
{
	if (inMaterial != NULL)
	{
		TArray<UTexture*> chainTextures;
		TArray<UTexture*> parentTextures;
		TArray<FName> parameterNames;
		TArray<FName>parentparameterNames;
		UTexture2D* testTexture;
		if (inMaterial->GetTexturesInPropertyChain(EMaterialProperty::MP_BaseColor, chainTextures, &parameterNames, NULL, ERHIFeatureLevel::Num,EMaterialQualityLevel::Num))
		{
			UMaterialInterface* myParent = ((UMaterialInstance*)inMaterial)->Parent;
			if (myParent != NULL)
			{

				// this is an instance
				myParent->GetTexturesInPropertyChain(EMaterialProperty::MP_BaseColor, parentTextures, &parentparameterNames, NULL, ERHIFeatureLevel::Num, EMaterialQualityLevel::Num);
				if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)UE_LOG(LogTemp, Log, TEXT("Material instance, filtering all parent textures"));
				for (int i = 0; i < parentTextures.Num(); i++)
					chainTextures.Remove(parentTextures[i]);
			}
			testTexture = NULL;

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
						if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)UE_LOG(LogTemp, Log, TEXT("Filtering textures..."));

						// cheaper than multiple gets? Dunno
						TArray<FString> TexBeginsWith = UHammUErDeveloperSettings::Get()->TextureBeginsWith;
						TArray<FString> TexContains = UHammUErDeveloperSettings::Get()->TextureContains;
						TArray<FString> TexEndsWith = UHammUErDeveloperSettings::Get()->TextureEndsWith;

						for (int i = 0; i < chainTextures.Num(); i++)
						{
							bool filterOK = true;
							FString testTextureName = chainTextures[i]->GetName();
							for (int j = 0; j < TexBeginsWith.Num(); j++)
							{
								if (testTextureName.StartsWith(TexBeginsWith[j], ESearchCase::Type::IgnoreCase))
								{
									filterOK = false;
									break;
								}
							}
							if (!filterOK)
								break;
							for (int j = 0; j < TexContains.Num(); j++)
							{
								if (testTextureName.Contains(TexContains[j], ESearchCase::Type::IgnoreCase))
								{
									filterOK = false;
									break;
								}
							}
							if (!filterOK)
								break;
							for (int j = 0; j < TexEndsWith.Num(); j++)
							{
								if (testTextureName.EndsWith(TexEndsWith[j], ESearchCase::Type::IgnoreCase))
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
						width = UHammUErDeveloperSettings::Get()->DefaultTextureSize.X;
						height = UHammUErDeveloperSettings::Get()->DefaultTextureSize.Y;

						return;
					}
				}

				width = testTexture->Source.GetSizeX();
				height = testTexture->Source.GetSizeY();
			}

		}
		else
		{
			width = UHammUErDeveloperSettings::Get()->DefaultTextureSize.X;
			height = UHammUErDeveloperSettings::Get()->DefaultTextureSize.Y;
		}
	}
	else
	{
		width = UHammUErDeveloperSettings::Get()->DefaultTextureSize.X;
		height = UHammUErDeveloperSettings::Get()->DefaultTextureSize.Y;
	}
}

bool HammUErSettings::GetMaterialSizesForChain(UMaterial* inMaterial, int& width, int& height, EMaterialProperty inProperty)
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
						if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)UE_LOG(LogTemp, Log, TEXT("Filtering textures..."));

						// cheaper than multiple gets? Dunno
						TArray<FString> TexBeginsWith = UHammUErDeveloperSettings::Get()->TextureBeginsWith;
						TArray<FString> TexContains = UHammUErDeveloperSettings::Get()->TextureContains;
						TArray<FString> TexEndsWith = UHammUErDeveloperSettings::Get()->TextureEndsWith;

						for (int i = 0; i < chainTextures.Num(); i++)
						{
							bool filterOK = true;
							FString testTextureName = chainTextures[i]->GetName();
							for (int j = 0; j < TexBeginsWith.Num(); j++)
							{
								if (testTextureName.StartsWith(TexBeginsWith[j], ESearchCase::Type::IgnoreCase))
								{
									filterOK = false;
									break;
								}
							}
							if (!filterOK)
								break;
							for (int j = 0; j < TexContains.Num(); j++)
							{
								if (testTextureName.Contains(TexContains[j], ESearchCase::Type::IgnoreCase))
								{
									filterOK = false;
									break;
								}
							}
							if (!filterOK)
								break;
							for (int j = 0; j < TexEndsWith.Num(); j++)
							{
								if (testTextureName.EndsWith(TexEndsWith[j], ESearchCase::Type::IgnoreCase))
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
						return false;
					}
				}

				width = testTexture->Source.GetSizeX();
				height = testTexture->Source.GetSizeY();
				return true;
			}

		}
		else
		{
			return false;
		}
	}
	return false;
}


// TODO: Move out to a helper class
UStaticMesh* HammUErSettings::CreateStaticMesh(struct FRawMesh& RawMesh, TArray<UMaterialInterface*>& Materials, UObject* InOuter, FName InName, bool noComplexCollision)
{
	// Create the UStaticMesh object.
	FlushRenderingCommands();
	FStaticMeshComponentRecreateRenderStateContext RecreateRenderStateContext(FindObject<UStaticMesh>(InOuter, *InName.ToString()));
	auto StaticMesh = NewObject<UStaticMesh>(InOuter, InName, RF_Public | RF_Standalone);

#if ENGINE_MAJOR_VERSION < 5
#if ENGINE_MINOR_VERSION < 27
	StaticMesh->LightingGuid = FGuid::NewGuid();
	StaticMesh->LightMapCoordinateIndex = 1;
	StaticMesh->LightMapResolution = UHammUErDeveloperSettings::Get()->LightmapPrecision; 
#else
	StaticMesh->SetLightingGuid(FGuid::NewGuid());
	StaticMesh->SetLightMapCoordinateIndex(1);
	StaticMesh->SetLightMapResolution(UHammUErDeveloperSettings::Get()->LightmapPrecision);  // make user selectable?
#endif

	// Add one LOD for the base mesh
	FStaticMeshSourceModel* SrcModel = new(StaticMesh->GetSourceModels()) FStaticMeshSourceModel();
	SrcModel->BuildSettings.bRecomputeNormals = false;
	SrcModel->BuildSettings.bRecomputeTangents = false;
	SrcModel->BuildSettings.bRemoveDegenerates = false;
	
	SrcModel->ReductionSettings.bRecalculateNormals = false;
	SrcModel->RawMeshBulkData->SaveRawMesh(RawMesh);
	SrcModel->StaticMeshOwner = StaticMesh;
#else
	StaticMesh->SetLightMapCoordinateIndex(1);
	StaticMesh->SetLightMapResolution(UHammUErDeveloperSettings::Get()->LightmapPrecision);  

	// Add one LOD for the base mesh
	FStaticMeshSourceModel SrcModel;
	SrcModel.CreateSubObjects(StaticMesh);
	SrcModel.BuildSettings.bRecomputeNormals = false;
	SrcModel.BuildSettings.bRecomputeTangents = false;
	SrcModel.BuildSettings.bRemoveDegenerates = false;
	SrcModel.ReductionSettings.bRecalculateNormals = false;
	SrcModel.SaveRawMesh(RawMesh);

	TArray<FStaticMeshSourceModel> temp;
	temp.Add(MoveTemp(SrcModel));

	StaticMesh->SetSourceModels(MoveTemp(temp));
#endif

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION < 27
	StaticMesh->StaticMaterials.Empty();
	for (int i = 0; i < Materials.Num(); i++)
		StaticMesh->StaticMaterials.Add(FStaticMaterial(Materials[i]));
#else
	TArray<FStaticMaterial> &materials = StaticMesh->GetStaticMaterials();
	materials.Empty();
	for (int i = 0; i < Materials.Num(); i++)
		materials.Add(FStaticMaterial(Materials[i]));
#endif

	int32 NumSections = Materials.Num();

	// Set up the SectionInfoMap to enable collision
	for (int32 SectionIdx = 0; SectionIdx < NumSections; ++SectionIdx)
	{
		FMeshSectionInfoMap& sectionInfoMap = StaticMesh->GetSectionInfoMap();
		FMeshSectionInfo Info = sectionInfoMap.Get(0, SectionIdx);
		Info.MaterialIndex = SectionIdx;
		Info.bEnableCollision = true;
		sectionInfoMap.Set(0, SectionIdx, Info);
	}

	TArray<FText> errorList;
	errorList.Empty();
	StaticMesh->Build(false, &errorList);
	if (errorList.Num() > 0)
	{
		for (int i = 0;i< errorList.Num();i++)
			UE_LOG(LogTemp, Error, TEXT("%s"), *(errorList[i].ToString()));
	}

	StaticMesh->CreateBodySetup();

	if (noComplexCollision)
	{
		int NumDirs = 18;
		TArray<FVector> Dirs;
		Dirs.AddUninitialized(NumDirs);
		for (int32 DirIdx = 0; DirIdx < NumDirs; ++DirIdx) { Dirs[DirIdx] = KDopDir18[DirIdx]; }
		GenerateKDopAsSimpleCollision(StaticMesh, Dirs);
	}
	else
	{
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION < 27
		UBodySetup* body = StaticMesh->BodySetup;
#else
		UBodySetup* body = StaticMesh->GetBodySetup();
#endif
		if (body)
		{
			if (body)
				body->CollisionTraceFlag = TEnumAsByte<ECollisionTraceFlag>(ECollisionTraceFlag::CTF_UseComplexAsSimple);
		}
	}

	for (int32 SectionIdx = 0; SectionIdx < NumSections; ++SectionIdx)
	{
		auto sectionInfoMap = StaticMesh->GetSectionInfoMap();
		FMeshSectionInfo Info = sectionInfoMap.Get(0, SectionIdx);
		Info.bEnableCollision = true;
		sectionInfoMap.Set(0, SectionIdx, Info);
	}

	StaticMesh->MarkPackageDirty();
	return StaticMesh;
}

void HammUErSettings::ReloadAssetDatabase()
{
	UE_LOG(LogTemp, Log, TEXT("Reloading asset database."));
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetData.Empty();
	AssetRegistryModule.Get().GetAssetsByClass("Material", AssetData, true);
	TArray<FAssetData> AssetData2;
	AssetRegistryModule.Get().GetAssetsByClass("MaterialInstance", AssetData2, true);
	AssetData.Append(AssetData2);
}

void HammUErSettings::LoadStaticMeshDatabase()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	StaticMeshAssetData.Empty();
	AssetRegistryModule.Get().GetAssetsByClass("StaticMesh", StaticMeshAssetData, true);
}

void HammUErSettings::LoadBPDatabase()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	BPAssetData.Empty();
	AssetRegistryModule.Get().GetAssetsByClass("Blueprint", BPAssetData, true);
}

UBlueprint* HammUErSettings::FindInBPDatabase(FString BPname)
{
	FString input = BPname.ToLower();
	if (BPAssetData.Num() == 0)
		LoadBPDatabase();
	FString test = "";
	if (BPAssetData.Num() > 0)
		for (int i = 0; i < BPAssetData.Num(); i++)
		{
			test = BPAssetData[i].AssetName.ToString().ToLower();
			if (test == input)
			{
				UBlueprint* found = (UBlueprint*)BPAssetData[i].GetAsset();
				return found;
			}
		}
	return NULL;
}

UStaticMesh* HammUErSettings::FindInStaticMeshDatabase(FString name)
{
	FString input = name;
	input.RemoveFromEnd(FPaths::GetExtension(input, true));
	if (StaticMeshAssetData.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Static mesh asset database wasn't loaded. Why."));
		LoadStaticMeshDatabase();
	}
	if (StaticMeshAssetData.Num()>0)
	{
		FString materialDirectory = "";

		name = name.Replace(TEXT("\\"), TEXT("/"));
		if (input.Contains("/"))
		{
			int32 pos = input.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
			materialDirectory = input.Mid(0, pos);
			if (materialDirectory.Contains("/"))
			{
				int second = materialDirectory.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
				materialDirectory = materialDirectory.Mid(second + 1) + "/";
				materialDirectory = "/" + materialDirectory.ToUpper();
			}
			input = input.Mid(pos + 1, input.Len() - pos);
			input = input.ToUpper();
		}
		if (materialDirectory != "")
		{
			FString FullSearch = materialDirectory + input;
			for (int i = 0; i < StaticMeshAssetData.Num(); i++)
			{
				FString origDir = StaticMeshAssetData[i].PackagePath.ToString().ToUpper();
				int slashIndex = origDir.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
				origDir = origDir.Mid(slashIndex);
				FString currentOne = origDir + "/" + StaticMeshAssetData[i].AssetName.ToString().ToUpper();
				if (currentOne == materialDirectory + input)
				{
					if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
						UE_LOG(LogTemp, Log, TEXT("Found SM %s for %s, tight sweep"), *currentOne, *(FullSearch));
					return (UStaticMesh*)StaticMeshAssetData[i].GetAsset();
				}
			}
			// okay, we're going to try AGAIN
			for (int i = 0; i < StaticMeshAssetData.Num(); i++)
			{
				FString currentOne = StaticMeshAssetData[i].PackagePath.ToString().ToUpper() + "/" + StaticMeshAssetData[i].AssetName.ToString().ToUpper();
				if ((currentOne.Contains(materialDirectory + input)))
				{
					if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
						UE_LOG(LogTemp, Log, TEXT("Found SM %s for %s, broad sweep"), *currentOne, *(FullSearch));
					return (UStaticMesh*)StaticMeshAssetData[i].GetAsset();
				}
			}
			// one last time, with the whole thing
			input = name.ToUpper();
			input.RemoveFromEnd(FPaths::GetExtension(input, true));
			for (int i = 0; i < StaticMeshAssetData.Num(); i++)
			{
				FString currentOne = StaticMeshAssetData[i].PackagePath.ToString().ToUpper() + "/" + StaticMeshAssetData[i].AssetName.ToString().ToUpper();
				if ((currentOne.Contains(input)))
				{
					if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
						UE_LOG(LogTemp, Log, TEXT("Found SM %s for %s, broad sweep"), *currentOne, *(FullSearch));
					return (UStaticMesh*)StaticMeshAssetData[i].GetAsset();
				}
			}
		}
		else
		{
			for (int i = 0; i < StaticMeshAssetData.Num(); i++)
			{
				FString currentOne = StaticMeshAssetData[i].AssetName.ToString().ToUpper();
				if ((currentOne == input)/*|| (currentOne == input) || (currentOne == input + "_MAT")*/)
				{
					if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
						UE_LOG(LogTemp, Log, TEXT("Found SM %s for %s, tight sweep"), *currentOne, *input);
					return (UStaticMesh*)StaticMeshAssetData[i].GetAsset();
				}
			}
			// okay, we're going to try AGAIN
			for (int i = 0; i < StaticMeshAssetData.Num(); i++)
			{
				FString currentOne = StaticMeshAssetData[i].AssetName.ToString().ToUpper();
				if ((currentOne.Contains(input))/* || (currentOne.Contains(input)) || (currentOne.Contains(input + "_MAT")*/)
				{
					if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
						UE_LOG(LogTemp, Log, TEXT("Found %s for %s, broad sweep"), *currentOne, *input);
					return (UStaticMesh*)StaticMeshAssetData[i].GetAsset();
				}
			}
		}
		if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
			UE_LOG(LogTemp, Warning, TEXT("Could not find anything in the asset database for Static Mesh %s"), *input);
		return NULL; // nope
	}
	else
	{
		if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
			UE_LOG(LogTemp, Warning, TEXT("Asset database empty?"));
		return NULL; // no asset database? What.
	}

}

UMaterial* HammUErSettings::MaterialInAssetDatabase(FString input)
{
	if (AssetData.Num() == 0)
	{
		ReloadAssetDatabase();
	}
	if (AssetData.Num() > 0)
	{
		FString materialDirectory ="";
		input = input.Replace(TEXT("\\"), TEXT("/"));
		if (input.Contains("/"))
		{
			int32 pos = input.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
			materialDirectory = input.Mid(0, pos).ToUpper();
			if (materialDirectory.Contains("/"))
			{
				int second = materialDirectory.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
				materialDirectory = materialDirectory.Mid(second + 1) + "/";
				materialDirectory = "/"+materialDirectory.ToUpper();
			}
			input = input.Mid(pos + 1, input.Len() - pos);
			input = input.ToUpper();
		}
		if (materialDirectory != "")
		{
			if (!materialDirectory.EndsWith("/"))
				materialDirectory += "/";
			FString FullSearch = materialDirectory + input;
			for (int i = 0; i < AssetData.Num(); i++)
			{
				FString origDir = AssetData[i].PackagePath.ToString().ToUpper();
				int slashIndex = origDir.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
				origDir = origDir.Mid(slashIndex);
				FString currentOne = origDir+"/"+AssetData[i].AssetName.ToString().ToUpper();
				if ((currentOne == materialDirectory + input) || (currentOne == materialDirectory + input + "_MAT")/*|| (currentOne == input) || (currentOne == input + "_MAT")*/)
				{
					if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
						UE_LOG(LogTemp, Log, TEXT("Found %s for %s, tight sweep"), *currentOne, *(FullSearch));
					return (UMaterial*)AssetData[i].GetAsset();
				}
			}
			// okay, we're going to try AGAIN
			for (int i = 0; i < AssetData.Num(); i++)
			{
				FString currentOne = AssetData[i].PackagePath.ToString().ToUpper() + "/" + AssetData[i].AssetName.ToString().ToUpper();
				if ((currentOne.Contains(materialDirectory + input)) || (currentOne.Contains(materialDirectory + input + "_MAT"))/* || (currentOne.Contains(input)) || (currentOne.Contains(input + "_MAT")*/)
				{
					if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
						UE_LOG(LogTemp, Log, TEXT("Found %s for %s, broad sweep"), *currentOne, *(FullSearch));
					return (UMaterial*)AssetData[i].GetAsset();
				}
			}
		}
		else
		{
			for (int i = 0; i < AssetData.Num(); i++)
			{
				FString currentOne = AssetData[i].AssetName.ToString().ToUpper();
				if ((currentOne == input) || (currentOne == input + "_MAT")/*|| (currentOne == input) || (currentOne == input + "_MAT")*/)
				{
					if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
						UE_LOG(LogTemp, Log, TEXT("Found %s for %s, tight sweep"), *currentOne, *input);
					return (UMaterial*)AssetData[i].GetAsset();
				}
			}
			// okay, we're going to try AGAIN
			for (int i = 0; i < AssetData.Num(); i++)
			{
				FString currentOne = AssetData[i].AssetName.ToString().ToUpper();
				if ((currentOne.Contains(input)) || (currentOne.Contains(input + "_MAT"))/* || (currentOne.Contains(input)) || (currentOne.Contains(input + "_MAT")*/)
				{
					if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
						UE_LOG(LogTemp, Log, TEXT("Found %s for %s, broad sweep"), *currentOne, *input);
					return (UMaterial*)AssetData[i].GetAsset();
				}
			}
		}
		if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
			UE_LOG(LogTemp, Warning, TEXT("Could not find anything in the asset database for Material %s"), *input);
		return NULL; // nope
	}
	else
	{
		if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
			UE_LOG(LogTemp, Warning, TEXT("Asset database empty?"));
		return NULL; // no asset database? What.
	}
}

int HammUErSettings::MaterialIndexAssetDatabase(FString input)
{
	if (AssetData.Num() == 0)
	{
		ReloadAssetDatabase();
	}
	if (AssetData.Num() > 0)
	{
		FString materialDirectory = "";
		input = input.Replace(TEXT("\\"), TEXT("/"));
		if (input.Contains("/"))
		{
			int32 pos = input.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
			materialDirectory = input.Mid(0, pos);
			if (materialDirectory.Contains("/"))
			{
				int second = materialDirectory.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
				materialDirectory = materialDirectory.Mid(second + 1) + "/";
				materialDirectory = "/" + materialDirectory.ToUpper();
			}
			input = input.Mid(pos + 1, input.Len() - pos);
			input = input.ToUpper();
		}
		if (materialDirectory != "")
		{
			FString FullSearch = materialDirectory + input;
			for (int i = 0; i < AssetData.Num(); i++)
			{
				FString origDir = AssetData[i].PackagePath.ToString().ToUpper();
				int slashIndex = origDir.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
				origDir = origDir.Mid(slashIndex);
				FString currentOne = origDir + "/" + AssetData[i].AssetName.ToString().ToUpper();
				if ((currentOne == materialDirectory + input) || (currentOne == materialDirectory + input + "_MAT")/*|| (currentOne == input) || (currentOne == input + "_MAT")*/)
				{
					if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
						UE_LOG(LogTemp, Log, TEXT("Found %s for %s, tight sweep"), *currentOne, *(FullSearch));
					return i;
				}
			}
			// okay, we're going to try AGAIN
			for (int i = 0; i < AssetData.Num(); i++)
			{
				FString currentOne = AssetData[i].PackagePath.ToString().ToUpper() + "/" + AssetData[i].AssetName.ToString().ToUpper();
				if ((currentOne.Contains(materialDirectory + input)) || (currentOne.Contains(materialDirectory + input + "_MAT"))/* || (currentOne.Contains(input)) || (currentOne.Contains(input + "_MAT")*/)
				{
					if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
						UE_LOG(LogTemp, Log, TEXT("Found %s for %s, broad sweep"), *currentOne, *(FullSearch));
					return i;
				}
			}
		}
		else
		{
			for (int i = 0; i < AssetData.Num(); i++)
			{
				FString currentOne = AssetData[i].AssetName.ToString().ToUpper();
				if ((currentOne == input) || (currentOne == input + "_MAT")/*|| (currentOne == input) || (currentOne == input + "_MAT")*/)
				{
					if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
						UE_LOG(LogTemp, Log, TEXT("Found %s for %s, tight sweep"), *currentOne, *input);
					return i;
				}
			}
			// okay, we're going to try AGAIN
			for (int i = 0; i < AssetData.Num(); i++)
			{
				FString currentOne = AssetData[i].AssetName.ToString().ToUpper();
				if ((currentOne.Contains(input)) || (currentOne.Contains(input + "_MAT"))/* || (currentOne.Contains(input)) || (currentOne.Contains(input + "_MAT")*/)
				{
					if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
						UE_LOG(LogTemp, Log, TEXT("Found %s for %s, broad sweep"), *currentOne, *input);
					return i;
				}
			}
		}
		if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
			UE_LOG(LogTemp, Warning, TEXT("Could not find anything in the asset database for Material %i"), *input);
		return -1; // nope
	}
	else
	{
		if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
			UE_LOG(LogTemp, Warning, TEXT("Asset database empty?"));
		return -1; // no asset database? What.
	}
}

FMaterialListEntry* HammUErSettings::GetMasterMaterialInfo(FString input)
{
	FString seeking = input.ToUpper();
	if (masterMaterialList.Num() > 0)
	{
		int index = FindInMaster(input);//masterMaterialList.IndexOfByPredicate([&](TSharedPtr<FMaterialListEntry> other) {return (other->oldName.ToUpper() == seeking) || (other->oldName.ToUpper().EndsWith("/" + seeking)); });

		if (index > -1)
		{
			return masterMaterialList[index].Get();
		}
		else
		{
			if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
				UE_LOG(LogTemp, Error, TEXT("Could not find %s in the master material list, returning empty material info. If your noclip is not being honoured, this is the problem."), *input);
			FMaterialListEntry* toReturn = new FMaterialListEntry();
			toReturn->oldName = input;
			toReturn->textureWidth = UHammUErDeveloperSettings::Get()->DefaultTextureSize.X;
			toReturn->textureHeight = UHammUErDeveloperSettings::Get()->DefaultTextureSize.Y;
			toReturn->newMaterial = NULL;
			toReturn->ignore = false;
			return toReturn;
		}
	}
	else
	{
		if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
			UE_LOG(LogTemp, Error, TEXT("Could not find %s in the master material list because it's empty, returning empty material info. If your noclip is not being honoured, this is the problem."), *input);
		FMaterialListEntry* toReturn = new FMaterialListEntry();
		toReturn->oldName = input;
		toReturn->textureWidth = UHammUErDeveloperSettings::Get()->DefaultTextureSize.X;
		toReturn->textureHeight = UHammUErDeveloperSettings::Get()->DefaultTextureSize.Y;
		toReturn->newMaterial = NULL;
		toReturn->ignore = false;
		return toReturn;
	}
}

int32 HammUErSettings::FindInMaster(FString tofind)
{
	FString tofindU = tofind.ToUpper();
	FString tofindMAT = tofindU + "_MAT";
	FString materialNameWithDir = "";
	FString materialNameWithDirMAT = "";
	FString materialName = tofind.ToUpper();
	FString materialNameMAT = tofindU + "_MAT";
	if (tofindU.Contains("/"))
	{
		int slashIndex = tofind.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		materialName = tofindU.Mid(slashIndex + 1);
		materialNameMAT = materialName + "_MAT";
		int slashIndex2 = tofind.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd, slashIndex - 1);
		materialNameWithDir = tofindU.Mid(slashIndex2 + 1);
		materialNameWithDirMAT = materialNameWithDir + "_MAT";
	}
	FString inputU;
	int index = HammUErSettings::masterMaterialList.IndexOfByPredicate([&](TSharedPtr<FMaterialListEntry> checkingAsset) {return (checkingAsset->oldName == tofindU) || (checkingAsset->oldName == tofindMAT); });
	if (index > -1)
		return index;
	if (materialNameWithDir != "")
	{
		if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
			UE_LOG(LogTemp, Warning, TEXT("Could not find %s, looking for %s"), *tofindU, *materialNameWithDir);
		index = HammUErSettings::masterMaterialList.IndexOfByPredicate([&](TSharedPtr<FMaterialListEntry> checkingAsset) {return (checkingAsset->oldName == materialNameWithDir) || (checkingAsset->oldName == materialNameWithDirMAT); });
		if (index > -1)
			return index;
		else
		{
			if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
				UE_LOG(LogTemp, Warning, TEXT("Could not find %s, looking for material names ending in /%s"), *tofindU, *tofindU);
			materialNameWithDir = "/" + materialNameWithDir;
			materialNameWithDirMAT = "/" + materialNameWithDirMAT;
			index = HammUErSettings::masterMaterialList.IndexOfByPredicate([&](TSharedPtr<FMaterialListEntry> checkingAsset) {return (checkingAsset->oldName.EndsWith(materialNameWithDir, ESearchCase::Type::IgnoreCase)) || (checkingAsset->oldName.EndsWith(materialNameWithDirMAT, ESearchCase::Type::IgnoreCase)); });
			if (index > -1)
				return index;
		}
	}
	if (materialName != "")
	{
		if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
			UE_LOG(LogTemp, Warning, TEXT("Could not find %s, looking for %s"), *tofindU, *materialName);
		index = HammUErSettings::masterMaterialList.IndexOfByPredicate([&](TSharedPtr<FMaterialListEntry> checkingAsset) {return (checkingAsset->oldName.Equals(materialName, ESearchCase::Type::IgnoreCase)) || (checkingAsset->oldName.Equals(materialNameMAT, ESearchCase::Type::IgnoreCase)); });
		if (index > -1)
			return index;
		else
		{
			if (UHammUErDeveloperSettings::Get()->VerboseDebugEnabled)
				UE_LOG(LogTemp, Warning, TEXT("Could not find %s, looking for material names ending in it"), *tofindU);
			tofindU = "/" + tofindU;
			tofindMAT = "/" + tofindMAT;
			index = HammUErSettings::masterMaterialList.IndexOfByPredicate([&](TSharedPtr<FMaterialListEntry> checkingAsset) {return (checkingAsset->oldName.EndsWith(tofindU, ESearchCase::Type::IgnoreCase)) || (checkingAsset->oldName.EndsWith(tofindMAT, ESearchCase::Type::IgnoreCase)); });
		}
	}
	return index;
}

void HammUErSettings::LoadInstanceDatabase()
{
	if (instanceDatabase.Num() == 0)
	{
		FString configName = FPaths::ProjectConfigDir() + "instance_data/" + "hammUErInstances.cfg";
		if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*(configName)))
		{
			TArray<FString> configLines;
			FFileHelper::LoadANSITextFileToStrings(*(configName), NULL, configLines);
			for (int i = 0; i < configLines.Num(); i++)
			{
				if (configLines[i] != "")
				{
					TArray<FString> brokenUp;
					int counter = 0;
					configLines[i].ParseIntoArray(brokenUp, TEXT(","), false);
					if (brokenUp.Num() == 2)
						instanceDatabase.Add(InstanceDatabaseEntry(brokenUp[0].ToLower(), brokenUp[1]));
				}
			}
			configLines.Empty();
		}
	}
}

void HammUErSettings::SaveInstanceDatabase()
{
	FString configName = FPaths::ProjectConfigDir() + "instance_data/"+ "hammUErInstances.cfg";
	FString SaveText = "";
	for (int i = 0; i < instanceDatabase.Num(); i++)
	{
		SaveText += instanceDatabase[i].instanceName.ToLower() + "," + instanceDatabase[i].fileName +"\r\n";
	}
	FFileHelper::SaveStringToFile(SaveText, *configName);
}

bool HammUErSettings::InstanceInDatabase(FString instance_name)
{
	if (instanceDatabase.Num() == 0)
		LoadInstanceDatabase();
	instance_name = instance_name.ToLower();
	for (int i = 0; i < instanceDatabase.Num(); i++)
	{
		if (instanceDatabase[i].instanceName == instance_name)
			return true;
	}
	if (instance_name.Contains("/"))
	{
		int index = instance_name.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		instance_name = instance_name.RightChop(index + 1);
		for (int i = 0; i < instanceDatabase.Num(); i++)
		{
			if (instanceDatabase[i].instanceName == instance_name)
				return true;
		}
	}
	instance_name = instance_name + ".vmf";
	for (int i = 0; i < instanceDatabase.Num(); i++)
	{
		if (instanceDatabase[i].instanceName == instance_name)
			return true;
	}
	return false;
}

void HammUErSettings::AddInstance(FString instance_name, FString instance_filename)
{
	if (instanceDatabase.Num() == 0)
		LoadInstanceDatabase();
	instanceDatabase.AddUnique(InstanceDatabaseEntry(instance_name, instance_filename));
	SaveInstanceDatabase();
}

FString HammUErSettings::GetInstance(FString instance_name)
{
	instance_name = instance_name.ToLower();
	for (int i = 0; i < instanceDatabase.Num(); i++)
	{
		if (instanceDatabase[i].instanceName == instance_name)
			return instanceDatabase[i].fileName;
	}
	if (instance_name.Contains("/"))
	{
		int index = instance_name.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		instance_name = instance_name.RightChop(index + 1);
		for (int i = 0; i < instanceDatabase.Num(); i++)
		{
			if (instanceDatabase[i].instanceName == instance_name)
				return instanceDatabase[i].fileName;
		}
	}
	instance_name = instance_name + ".vmf";
	for (int i = 0; i < instanceDatabase.Num(); i++)
	{
		if (instanceDatabase[i].instanceName == instance_name)
			return instanceDatabase[i].fileName;
	}

	return "not_found";
}