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
#include "HammUErSettings.h"
#include "CoreMinimal.H"
#include "Editor.H"

#include "Runtime/slate/public/widgets/views/slistview.h"

#include "Runtime/Engine/Classes/Engine/selection.H"
#include "Runtime/Appframework/public/widgets/colors/SColorPicker.h"
#include "Runtime/slate/public/widgets/notifications/snotificationlist.h"
#include "Runtime/slate/public/framework/notifications/notificationmanager.h"
/**
 * 
 */
class SHammerSettingsWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SHammerSettingsWidget)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);


	enum ERadioChoice
	{
		Radio0,
		Radio1,
		Radio2,
		Radio3,
		ScaleRadio0,
		ScaleRadio1,
		ScaleRadio2
	};


private:

	// settings page code
	ECheckBoxState GetDebugCheckedActive() const
	{
		return HammUErSettings::DebugCheckboxEnabled ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}

	void OnDebugCheckboxChanged(ECheckBoxState NewState)
	{
		HammUErSettings::DebugCheckboxEnabled = (NewState == ECheckBoxState::Checked);
	}

	// other settings
	ECheckBoxState GetVMFList() const
	{
		return HammUErSettings::generateVMFList ? ECheckBoxState::Unchecked : ECheckBoxState::Checked;
	}

	void OnVMFListChanged(ECheckBoxState NewState)
	{
		HammUErSettings::generateVMFList = (NewState == ECheckBoxState::Unchecked);
	}


	ECheckBoxState GetIgnoreActors() const
	{
		return HammUErSettings::checkIgnore ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}

	void SetIgnoreActors(ECheckBoxState NewState)
	{
		HammUErSettings::checkIgnore = (NewState == ECheckBoxState::Checked);
	}



	FReply TagSelectedAsIgnore()
	{
		USelection* selected = GEditor->GetSelectedActors();
		for (int i = 0; i < selected->Num(); i++)
			((AActor*)selected->GetSelectedObject(i))->Tags.AddUnique(FName(*FString("HammUErIgnoreOnImport")));
		return FReply::Handled();
	}


	FReply UntagSelected()
	{
		USelection* selected = GEditor->GetSelectedActors();
		for (int i = 0; i < selected->Num(); i++)
			((AActor*)selected->GetSelectedObject(i))->Tags.Remove(FName(*FString("HammUErIgnoreOnImport")));
		return FReply::Handled();
	}


	FReply GetSelectedOnes()
	{
		USelection* selected = GEditor->GetSelectedActors();
		for (int i = 0; i < selected->Num(); i++)
		{
			FString name = selected->GetSelectedObject(i)->GetName();
			int index = FCString::Atoi(*name.Mid(name.Find("_", ESearchCase::IgnoreCase, ESearchDir::FromEnd)+1, name.Len()));
			if (name.Contains("_singlemesh_") || name.Contains("_groupmesh_") || name.Contains("_entity_mesh_")) // temporary, but since they're still uncoupled...
				HammUErSettings::reducedImportList.AddUnique(index);
		}
		HammUErSettings::reducedImportList.Sort();
		return FReply::Handled();
	}

	FReply GetSelectedEntitiesOnes()
	{
		USelection* selected = GEditor->GetSelectedActors();
		for (int i = 0; i < selected->Num(); i++)
		{
			if (((AActor*)selected->GetSelectedObject(i))->Tags.Num()>0)
			{
				FString name = ((AActor*)selected->GetSelectedObject(i))->Tags[0].ToString();
				int index = FCString::Atoi(*name.Mid(name.Find("_", ESearchCase::IgnoreCase, ESearchDir::FromEnd) + 1, name.Len()));
				if (name.StartsWith("Entity_"))
					HammUErSettings::reducedImportEntityList.AddUnique(index);
			}
		}
		HammUErSettings::reducedImportEntityList.Sort();
		return FReply::Handled();
	}
	
	// radio button 
	// Holds the current choice in the SCheckBox (as radio button) example.
	ERadioChoice RadioChoice;

	FReply SaveSettings()
	{
		FNotificationInfo Info(FText::FromString("Saving settings. This might take some time."));
		Info.bUseThrobber = true;
		Info.ExpireDuration = 3.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
		FSlateNotificationManager::Get().Tick();
	
		HammUErSettings::LoadConfig(true);
		Info.ExpireDuration = 3.0f;
		HammUErSettings::SaveConfig();

		return FReply::Handled();
	}


	void SetSmoothAngle(const float inRate);
	TOptional<float> GetSmoothAngle() const;

	void SetLightmapSize(const float inRate);
	TOptional<float> GetLightmapSize() const;

	void OnImportListChanged(const FText& InLabel, ETextCommit::Type TextType);
	FText GetImportList() const;

	void OnImportEntityListChanged(const FText& InLabel, ETextCommit::Type TextType);
	FText GetImportEntityList() const;

	// dirty hacks, get your dirty hacks here!
	ECheckBoxState GetIgnorePitch() const
	{
		return HammUErSettings::ignorePitch ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}

	void SetIgnorePitch(ECheckBoxState NewState)
	{
		HammUErSettings::ignorePitch = (NewState == ECheckBoxState::Checked);
	}

	ECheckBoxState GetIgnoreRoll() const
	{
		return HammUErSettings::ignoreRoll ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}

	void SetIgnoreRoll(ECheckBoxState NewState)
	{
		HammUErSettings::ignoreRoll = (NewState == ECheckBoxState::Checked);
	}

	ECheckBoxState GetIgnoreYaw() const
	{
		return HammUErSettings::ignoreYaw ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}

	void SetIgnoreYaw(ECheckBoxState NewState)
	{
		HammUErSettings::ignoreYaw = (NewState == ECheckBoxState::Checked);
	}

	ECheckBoxState GetDegenerate() const
	{
		return HammUErSettings::checkDegenerates ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}

	void SetDegenerate(ECheckBoxState NewState)
	{
		HammUErSettings::checkDegenerates = (NewState == ECheckBoxState::Checked);
	}


};
