// HammUEr
// Copyright 2015-2021 Turfster / NT Entertainment
// All Rights Reserved.


// This code is held together with kitten whiskers and toothpicks.
// There's nothing to see or learn here.
// Seriously.
// Turn back.

#pragma once

#include "HammUErPrivatePCH.h"
#include "HammUEr.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Notifications/SProgressbar.h"
#include "AssetRegistryModule.h"
#include "../../ThirdParty/VTFLib/VTFLib.h"
#include "HammUErSettings.h"


enum VMTShaderType :int
{
	Undefined = -1,
	LightmappedGeneric = 0,
	VertexLitGeneric,
	UnlitGeneric,
	WorldVertexTransition
};

struct VMTInfo
{
	VMTShaderType shader;
	FString VMTName;
	FString baseTexture;
	FString baseTexture2;
	FString normalTexture;
	FString normalTexture2;
	FString specularTexture;
	FString surfaceType;
	bool isTranslucent;
	bool isMasked;
	bool isDecal;
	float decalScale;
	bool hasEnvMap;
	bool isSelfIllum;
	float seamlessScale;

	VMTInfo()
	{
		shader = VMTShaderType::Undefined;
		baseTexture = "";
		baseTexture2 = "";
		normalTexture = "";
		normalTexture2 = "";
		specularTexture = "";
		VMTName = "";
		surfaceType = "";
		isTranslucent = false;
		isMasked = false;
		isDecal = false;
		decalScale = 1.f;
		hasEnvMap = false;
		isSelfIllum = false;
		seamlessScale = 1.f;
	}
};

struct ImporterTextureInfo
{
public: 
	FString textureLocation;
//	UTexture2D* texture;
	int width;
	int height;

	ImporterTextureInfo(/*UTexture2D*/FString tex, int w, int h)
	{
		textureLocation = tex;
		width = w;
		height = h;
	}
};

class SHammerTextUErWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SHammerTextUErWidget)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	
private:
	TSharedPtr<SProgressBar> myProgressbar;
	float completionPercentage;

	int32 currentMaterial;
	void DoNextMaterial();
	void ExportMaterial(UMaterialInterface* workMaterial, FString newMatName, int index);
	FReply DoExportOnly();
	
	TArray<FAssetData> AssetData;
	bool running = false;

	bool CanExport() const
	{
		return HammUErSettings::VTFExportDirectory != "";
	}

	FString identifier = "DEADBEEFCAFE";

	FReply HandleChooseFileButtonClicked();
	FReply ExportAllTextures();
	FText GetExportDirectory() const;
	TOptional<float> GetPercentage() const;

	bool shrinkTextures = false;  // experiment failed, kinda, so let's just ignore this

	void OnBeginsWithChanged(const FText& InLabel, ETextCommit::Type TextType);
	FText GetBeginsWithList() const;

	void OnKeepContainsWithChanged(const FText& InLabel, ETextCommit::Type TextType);
	FText GetKeepContainsWithList() const;

	void OnContainsChanged(const FText& InLabel, ETextCommit::Type TextType);
	FText GetContainsList() const;

	void OnEndsWithChanged(const FText& InLabel, ETextCommit::Type TextType);
	FText GetEndsWithList() const;

	void TextureDirectoryChanged(const FText& InLabel, ETextCommit::Type TextType);
	FText GetTextureImportDirectory() const;

	// discovery
	bool discovering = false;

	bool CanDiscover() const;
	FReply DoDiscovery();
	void DoNextDiscovery();

	ECheckBoxState GetRebuild() const
	{
		return HammUErSettings::discoverRebuild ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}

	void OnRebuildChanged(ECheckBoxState NewState)
	{
		HammUErSettings::discoverRebuild = (NewState == ECheckBoxState::Checked);
	}

	// importer
	UMaterialInterface* sourceMaterial;
	UMaterialInterface* decalMaterial;

	FString GetCurrentPath() const;
	void OnMaterialChanged(const FAssetData& inAssetData);

	FString GetDecalCurrentPath() const;
	void OnDecalMaterialChanged(const FAssetData& inAssetData);

	bool importRunning = false;
	TArray<FString> PendingImportFiles;
	//int32 currentImportFile = 0;

	bool CanImport() const;
	FReply GetImportDir();

	bool CanRunImport() const;
	FReply RunImport();

	void ImportVTF(FString fileName);
	void ImportVMT(FString fileName);
	void ImportMTR(FString fileName);
	void ImportSHADER(FString fileName);
	void ImportOther(FString fileName);
	void ImportTGA(FString fileName);
	void ImportWAD(FString fileName);
	void ImportWAL(FString fileName);

	TArray<TSharedPtr<FString>> parameterDropNamesList;
	TSharedPtr<FString> InitialSelected;
	FString currentParameter;
	FString currentBlendParameter;
	
	TArray<TSharedPtr<FString>> decalParameterDropNamesList;
	TSharedPtr<FString> InitialDecalSelected;
	FString decalParameter;

	bool matContainsNormal = false;
	TArray<TSharedPtr<FString>> normalParameterDropNamesList;
	TSharedPtr<FString> InitialNormalSelected;
	FString currentNormalParameter;
	FString currentBlendNormalParameter;

	bool matContainsSpecular = false;
	TArray<TSharedPtr<FString>> specularParameterDropNamesList;
	TSharedPtr<FString> InitialSpecularSelected;
	FString currentSpecularParameter;

	FText GetImportFolderName() const
	{
		return FText::FromString(HammUErSettings::textureSourceDirectory);
	}

	FText HandleComboText() const
	{
		return FText::FromString(currentParameter);
	}

	FText HandleDecalComboText() const
	{
		return FText::FromString(decalParameter);
	}


	TSharedRef<SWidget> HandleComboGenerateWidget(TSharedPtr<FString> inItem)
	{
		return SNew(STextBlock)
			.Text(FText::FromString(*inItem));
	}

	void HandleComboSelectionChanged(TSharedPtr<FString> inSelection, ESelectInfo::Type SelectInfo)
	{
		if (inSelection.IsValid())
			currentParameter = *inSelection;
	}

	void HandleDecalComboSelectionChanged(TSharedPtr<FString> inSelection, ESelectInfo::Type SelectInfo)
	{
		if (inSelection.IsValid())
			decalParameter = *inSelection;
	}

	bool HasNormalParameter() const
	{
		return matContainsNormal;
	}

	FText HandleNormalComboText() const
	{
		return FText::FromString(currentNormalParameter);
	}

	TSharedRef<SWidget> HandleNormalComboGenerateWidget(TSharedPtr<FString> inItem)
	{
		return SNew(STextBlock)
			.Text(FText::FromString(*inItem));
	}

	void HandleNormalComboSelectionChanged(TSharedPtr<FString> inSelection, ESelectInfo::Type SelectInfo)
	{
		if (inSelection.IsValid())
			currentNormalParameter = *inSelection;
	}

	EVisibility GetNormalVisibility() const
	{
		if (matContainsNormal)
			return EVisibility::Visible;
		else
			return EVisibility::Hidden;
	}

	void OnNormalsEndChanged(const FText& InLabel, ETextCommit::Type TextType);
	FText GetNormalsEndList() const;

	bool HasSpecularParameter() const
	{
		return matContainsSpecular;
	}

	FText HandleSpecularComboText() const
	{
		return FText::FromString(currentSpecularParameter);
	}

	TSharedRef<SWidget> HandleSpecularComboGenerateWidget(TSharedPtr<FString> inItem)
	{
		return SNew(STextBlock)
			.Text(FText::FromString(*inItem));
	}

	void HandleSpecularComboSelectionChanged(TSharedPtr<FString> inSelection, ESelectInfo::Type SelectInfo)
	{
		if (inSelection.IsValid())
			currentSpecularParameter = *inSelection;
	}

	EVisibility GetSpecularVisibility() const
	{
		if (matContainsSpecular)
			return EVisibility::Visible;
		else
			return EVisibility::Hidden;
	}

	void OnIgnoreEndChanged(const FText& InLabel, ETextCommit::Type TextType);
	FText GetIgnoreEndList() const;

	TArray<VMTInfo*> importerVMTList;
	TArray<FString> importerMaterialsDone;
	TMap<FString, ImporterTextureInfo> importerTextureList;
	TArray<FString> importerTextureNames;

	bool ImporterTextureListContains(FString textureName, FString &foundName);
	bool ImporterMaterialsDoneContains(FString textureName);
	
	void ReloadTextureDatabase();
	bool TextureDatabaseContains(FString textureName, FString &foundName);
	UTexture2D* GetTextureFromAssetDatabase(FString textureName);

	void CreateTexture(int32 width, int32 height, TArray<FColor>& colorArray, FString ObjectName, bool hasAlpha);
	void CreateMaterial(int i);
	void CreateMaterials();
	
	TArray<FString> failedMats;

	int createdTextures;
	TArray<UPackage*> unloadtest;
	TArray<FAssetData> TextureAssets;

	enum ERadioChoice
	{
		Radio0,
		Radio1,
		Radio2
	};
	
	ERadioChoice RadioChoice;

	ECheckBoxState HandleRadioButtonIsChecked(ERadioChoice ButtonId) const
	{
		return (HammUErSettings::importTexturesMats == (int)ButtonId)
		? ECheckBoxState::Checked
		: ECheckBoxState::Unchecked;
	}

	void HandleRadioButtonCheckStateChanged(ECheckBoxState NewRadioState, ERadioChoice RadioThatChanged)
	{
		if (NewRadioState == ECheckBoxState::Checked)
			HammUErSettings::importTexturesMats = (int)RadioThatChanged;// Choice;
	}

	TSharedRef<SWidget> CreateRadioButton(const FText& RadioText, ERadioChoice RadioButtonChoice)
	{
		return SNew(SCheckBox)
			.Style(FCoreStyle::Get(), "RadioButton")
			.IsChecked(this, &SHammerTextUErWidget::HandleRadioButtonIsChecked, RadioButtonChoice)
			.OnCheckStateChanged(this, &SHammerTextUErWidget::HandleRadioButtonCheckStateChanged, RadioButtonChoice)
			[
				SNew(STextBlock)
				.Text(RadioText)
			];
	}

};

