// HammUEr
// Copyright 2015-2020 Turfster / NT Entertainment
// All Rights Reserved.


// This code is held together with kitten whiskers and toothpicks.
// There's nothing to see or learn here.
// Seriously.
// Turn back.

#pragma once
#include "HammUErPrivatePCH.h"

#include "HammUEr.h"
#include "MapFileLoader.h"

#include "Widgets/Docking/SDockTab.h"
#include "HammUErSettings.h"
#include "Runtime/Launch/Resources/Version.h"  // needed to catch the oldass 4.10 'we didn't actually think this through and didn't supply a directory version of our picker' 

#include "Dialogs/DlgPickPath.h"

#include "Runtime/slate/public/widgets/views/slistview.h"
#include "Runtime/slate/public/widgets/input/seditabletextbox.h"

class SHammerDialogWidget : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SHammerDialogWidget) {}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

protected:
	TArray<TSharedPtr<FMaterialListEntry>> materialList;
	TArray<int32> materialListIndex;

	TSharedPtr<SListView<TSharedPtr<FMaterialListEntry>>> materialListWidget;

	TSharedPtr<SEditableTextBox> importDirBlock;
	TSharedPtr<SEditableTextBox> importSeekBlock;

private:


	FString versionString = "2.3 Take Five 20220506-2100\r\n(c) 2015-2022 NT Entertainment";

	FReply RunConvert();
	FReply RunFirstPhase();
	FReply RunToggle();

	FReply HandleChooseFileButtonClicked();

	FString VMFileName;
	float DurationActive;

	bool firstPhaseHasRun = false;

	void WriteMaterialFile();
	void ReadMaterialFile();

	void OnFileNameChanged(const FText& InLabel);
	FText GetVMFileName() const;

	bool CanConvert() const;
	bool CanRunFirstPhase() const;
	bool CanChooseFile() const;
	bool CanToggle() const;

	TSharedRef<ITableRow> MakeListViewWidget(TSharedPtr<FMaterialListEntry> Item, const TSharedRef<STableViewBase>& OwnerTable);

	void OnImportDirNameChanged(const FText& InLabel);
	FText GetImportDirName() const;

	FString importDirName;

	void OnSeekNameChanged(const FText& InLabel, ETextCommit::Type TextType);
	FText GetSeekName() const;
	FString seekName;

	EVisibility GetSeekVisibility() const
	{
		if (firstPhaseHasRun)
			return EVisibility::Visible;
		else
			return EVisibility::Hidden;
	}

	FReply GetImportHeader()
	{
		TSharedPtr<SDlgPickPath> PickAssetPathWidget =
			SNew(SDlgPickPath)
			.Title(FText::FromString("Select directory to save map in"));

		if (EAppReturnType::Ok == PickAssetPathWidget->ShowModal())
		{
			importDirName = PickAssetPathWidget->GetPath().ToString();
			importDirName.RemoveFromStart("/Game/");
			HammUErSettings::defaultImportHeader = importDirName;
			importDirBlock->SetText(FText::FromString(importDirName));
		}
		return FReply::Handled();
	}

	FReply ResetImportHeader()
	{
		importDirName = "";
		HammUErSettings::defaultImportHeader = importDirName;
		importDirBlock->SetText(FText::FromString(importDirName));
		return FReply::Handled();
	}


	ECheckBoxState GetGenerateMeshesActive() const
	{
		return HammUErSettings::doNotGenerateMeshes ? ECheckBoxState::Unchecked : ECheckBoxState::Checked;
	}

	void OnGenerateMeshesChanged(ECheckBoxState NewState)
	{
		HammUErSettings::doNotGenerateMeshes = !(NewState == ECheckBoxState::Checked);
	}

	ECheckBoxState GetGenerateEntitiesActive() const
	{
		return HammUErSettings::doNotGenerateEntities ? ECheckBoxState::Unchecked : ECheckBoxState::Checked;
	}

	void OnGenerateEntitiesChanged(ECheckBoxState NewState)
	{
		HammUErSettings::doNotGenerateEntities = !(NewState == ECheckBoxState::Checked);
		if (HammUErSettings::doNotGenerateEntities)  // unknown entities are also entities, so
			HammUErSettings::doNotGenerateUnknownNotes = true;
	}

	ECheckBoxState GetGenerateUnknownsActive() const
	{
		return HammUErSettings::doNotGenerateUnknownNotes ? ECheckBoxState::Unchecked : ECheckBoxState::Checked;
	}

	void OnGenerateUnknownsChanged(ECheckBoxState NewState)
	{
		HammUErSettings::doNotGenerateUnknownNotes = !(NewState == ECheckBoxState::Checked);
	}


	FReply RunInstance();
	bool createInstanceData = false;

};