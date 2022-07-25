// HammUEr
// Copyright 2015-2021 Turfster / NT Entertainment
// All Rights Reserved.


// This code is held together with kitten whiskers and toothpicks.
// There's nothing to see or learn here.
// Seriously.
// Turn back.

#include "SHammerSettingsWidget.h"
#include "SlateOptMacros.h"

#include "HammUEr.h"
#include "HammUErSettings.h"

#include "Runtime/Slate/Public/SlateFwd.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SRotatorInputBox.h"
#include "Widgets/Input/SVectorinputBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Layout/SHeader.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"
#include "Editor/UnrealEd/Classes/Factories/MaterialFactoryNew.h"

#define LOCTEXT_NAMESPACE "SHammerSettingsWidget"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SHammerSettingsWidget::Construct(const FArguments& InArgs)
{
	FString imagePath = FPaths::ProjectPluginsDir() + TEXT("HammUEr/Resources/headertest.png");
	if (!FPaths::FileExists(imagePath))
		imagePath = FPaths::EnginePluginsDir() + TEXT("HammUEr/Resources/headertest.png");
	FName brushName = FName(*imagePath);
	ChildSlot
		[
			SNew(SBox)
			.WidthOverride(400)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(3.0f, 1.0f)
				[
					SNew(SBox)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)

					.WidthOverride(400)
					.MaxDesiredWidth(400)
					.HeightOverride(40)
					[
						SNew(SImage)
						.Image(new FSlateDynamicImageBrush(brushName, FVector2D(400, 40), FLinearColor::White, ESlateBrushTileType::NoTile, ESlateBrushImageType::FullColor))
					]
				]
				+ SVerticalBox::Slot()
					.Padding(3.0f, 1.0f)
					[
						SNew(SBorder)
						.Padding(FMargin(3))
						[
							SNew(SBox)
							.Padding(FMargin(3))
							[
								SNew(SVerticalBox)
								+ SVerticalBox::Slot()
									.AutoHeight()
									[
										SNew(SHeader)
										.Content()
										[
											SNew(STextBlock)
											.Text(LOCTEXT("sessionsettings","Session specific settings"))
										]
									]
								+ SVerticalBox::Slot()
									.AutoHeight()
									[
										SNew(SBorder)
										[
											SNew(SVerticalBox)
											+ SVerticalBox::Slot()
											.AutoHeight()
											.Padding(FMargin(3.0f, 1.0f))
											[
												SNew(SHorizontalBox)
												+ SHorizontalBox::Slot()
												.AutoWidth()
												[
													SNew(SBox)
													.WidthOverride(450)
													[
														SNew(SHorizontalBox)
														+ SHorizontalBox::Slot()
														.AutoWidth()
														.Padding(FMargin(3.0f, 1.0f))
														.VAlign(VAlign_Center)
														[
															SNew(STextBlock)
															.Text(LOCTEXT("Ignore_rotations", "Ignore rotations when placing models"))
															.ToolTipText(LOCTEXT("ignoreRotationsTooltip", "This is one of those toggles you probably won't need, but since the rotation stuff is kinda hacky,\r\nsometimes you might want to just ignore one of these, possibly pitch."))
														]
														+ SHorizontalBox::Slot()
														.MaxWidth(20)
														.AutoWidth()
														[
															SNew(SCheckBox)
															.IsChecked(this, &SHammerSettingsWidget::GetIgnorePitch)
															.OnCheckStateChanged(this, &SHammerSettingsWidget::SetIgnorePitch)
															.ToolTipText(LOCTEXT("ignorePitchTooltip", "Ignore Pitch"))
														]
														+ SHorizontalBox::Slot()
														.MaxWidth(20)
														.AutoWidth()
														[
															SNew(SCheckBox)
															.IsChecked(this, &SHammerSettingsWidget::GetIgnoreRoll)
															.OnCheckStateChanged(this, &SHammerSettingsWidget::SetIgnoreRoll)
															.ToolTipText(LOCTEXT("ignoreRollTooltip", "Ignore Roll"))
														]
														+ SHorizontalBox::Slot()
														.MaxWidth(20)
														.AutoWidth()
														[
															SNew(SCheckBox)
															.IsChecked(this, &SHammerSettingsWidget::GetIgnoreYaw)
															.OnCheckStateChanged(this, &SHammerSettingsWidget::SetIgnoreYaw)
															.ToolTipText(LOCTEXT("ignoreYawTooltip", "Ignore Yaw"))
														]
														+ SHorizontalBox::Slot()
														.MaxWidth(30)
															.AutoWidth()
															.Padding(FMargin(3.0f, 1.0f))
															.VAlign(VAlign_Center)
															[
																SNew(STextBlock)
																.Text(LOCTEXT("Empty",""))
															]
														+SHorizontalBox::Slot()
															.MaxWidth(150)
															.AutoWidth()
															.Padding(FMargin(3.0f, 1.0f))
															.VAlign(VAlign_Center)
															[
																SNew(STextBlock)
																.Text(LOCTEXT("checkDegenerate","Check for degenerate tris"))
															]
														+ SHorizontalBox::Slot()
															.MaxWidth(20)
															.AutoWidth()
															[
																SNew(SCheckBox)
																.IsChecked(this, &SHammerSettingsWidget::GetDegenerate)
															.OnCheckStateChanged(this, &SHammerSettingsWidget::SetDegenerate)
															.ToolTipText(LOCTEXT("checkDegenerateTooltip", "Additional check for degenerate triangles before passing them to UE.\r\nShouldn't usually need this."))
															]
													]
												]
												+SHorizontalBox::Slot()
												.AutoWidth()
												.VAlign(VAlign_Center)
												[
													SNew(STextBlock)
													.Text(LOCTEXT("DoIgnoreLabel", " Ignore tagged "))
												]
												+ SHorizontalBox::Slot()
												.AutoWidth()
												[
													SNew(SBox)
													.WidthOverride(20)
													[
														SNew(SCheckBox)
														.IsChecked(this, &SHammerSettingsWidget::GetIgnoreActors)
														.OnCheckStateChanged(this, &SHammerSettingsWidget::SetIgnoreActors)
														.ToolTipText(LOCTEXT("ignoreIgnoreActorsTooltip","When checked, ignores actors tagged to be ignored"))
													]
												]
												+ SHorizontalBox::Slot()
												.AutoWidth()
												[
													SNew(SBox)
													[
														SNew(SButton)
														.OnClicked(this, &SHammerSettingsWidget::TagSelectedAsIgnore)
														.IsEnabled(true)
														.HAlign(HAlign_Center)
														.VAlign(VAlign_Center)
														.Text(LOCTEXT("TagIgnoreSelectedMeshes", "Tag"))
														.ToolTipText(LOCTEXT("TagIgnoreSelectedTooltip", "Tags current selection to ignore"))
													]
												]
												+ SHorizontalBox::Slot()
												.AutoWidth()
												[
													SNew(SBox)
													[
														SNew(SButton)
														.OnClicked(this, &SHammerSettingsWidget::UntagSelected)
														.IsEnabled(true)
														.HAlign(HAlign_Center)
														.VAlign(VAlign_Center)
														.Text(LOCTEXT("UntagSelected", "Untag"))
														.ToolTipText(LOCTEXT("UnTagIgnoreSelectedTooltip", "Untags current selection"))
													]
												]

											]
									+ SVerticalBox::Slot()
									.AutoHeight()
									.Padding(FMargin(3.f, 1.f))
									[
										SNew(SHorizontalBox)
										+ SHorizontalBox::Slot()
										.AutoWidth()
										.VAlign(VAlign_Center)
										[
											SNew(STextBlock)
											.Text(LOCTEXT("Import_smallerlist", "Load only (brushes & groups): "))
										]
										+ SHorizontalBox::Slot()
										.VAlign(VAlign_Center)
										[
											SNew(SEditableTextBox)
											.BackgroundColor(FSlateColor(FLinearColor(FColor::White)))
											.OnTextCommitted(this, &SHammerSettingsWidget::OnImportListChanged)
											.Text(this, &SHammerSettingsWidget::GetImportList)
											.ToolTipText(LOCTEXT("smallerlisttooltip","Comma separated list of brushes or groups"))
										]
										+ SHorizontalBox::Slot()
										.VAlign(VAlign_Center)
										.AutoWidth()
										[
											SNew(SButton)
											.OnClicked(this, &SHammerSettingsWidget::GetSelectedOnes)
											.IsEnabled(true)
											.HAlign(HAlign_Center)
											.VAlign(VAlign_Center)
											.Text(LOCTEXT("GetSelection1", "Get Selection"))
											.ToolTipText(LOCTEXT("GetSelectiontooltip", "Grab all brush meshes from selection."))
										]
									]
								+ SVerticalBox::Slot()
								.AutoHeight()
								.Padding(FMargin(3.f, 1.f))
								[
									SNew(SHorizontalBox)
									+ SHorizontalBox::Slot()
									.AutoWidth()
									.VAlign(VAlign_Center)
									[
										SNew(STextBlock)
										.Text(LOCTEXT("Import_smallerlistEntity", "Load only (entities): "))
									]
								+ SHorizontalBox::Slot()
									.VAlign(VAlign_Center)
									[
										SNew(SEditableTextBox)
										.BackgroundColor(FSlateColor(FLinearColor(FColor::White)))
									.OnTextCommitted(this, &SHammerSettingsWidget::OnImportEntityListChanged)
									.Text(this, &SHammerSettingsWidget::GetImportEntityList)
									.ToolTipText(LOCTEXT("smallerlistEntitytooltip", "Comma separated list of entities"))
									]
								+ SHorizontalBox::Slot()
									.VAlign(VAlign_Center)
									.AutoWidth()
									[
										SNew(SButton)
										.OnClicked(this, &SHammerSettingsWidget::GetSelectedEntitiesOnes)
										.IsEnabled(true)
										.HAlign(HAlign_Center)
										.VAlign(VAlign_Center)
										.Text(LOCTEXT("GetSelection2", "Get Selection"))
										.ToolTipText(LOCTEXT("GetSelection2tooltip", "Grab all entity meshes from selection."))
									]
								]
							]
						]
					]
				]
			]
		]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION


void SHammerSettingsWidget::OnImportListChanged(const FText& InLabel, ETextCommit::Type TextType)
{
	if (TextType == ETextCommit::OnEnter || TextType == ETextCommit::Default || TextType == ETextCommit::OnUserMovedFocus)
	{
		FString newList = InLabel.ToString();
		FString temp;
		int32 pos;
		newList.TrimStartInline();
		newList.TrimEndInline();
		HammUErSettings::reducedImportList.Empty();
		while (newList.Len() > 0)
		{
			pos = newList.Find(",");
			if (pos == -1)
				pos = newList.Len();
			temp = newList.Mid(0, pos);
			temp.TrimEndInline();
			int32 found = FCString::Atoi(*temp);
			HammUErSettings::reducedImportList.Add(found);
			newList.RemoveAt(0, pos + 1);
		}
	}
}

FText SHammerSettingsWidget::GetImportList() const
{
	FString result = "";
	for (int i = 0; i < HammUErSettings::reducedImportList.Num(); i++)
		result += (FString::FromInt(HammUErSettings::reducedImportList[i]) + (i == HammUErSettings::reducedImportList.Num() - 1 ? "" : ","));
	return FText::FromString(result);
}

void SHammerSettingsWidget::OnImportEntityListChanged(const FText& InLabel, ETextCommit::Type TextType)
{
	if (TextType == ETextCommit::OnEnter || TextType == ETextCommit::Default || TextType == ETextCommit::OnUserMovedFocus)
	{
		FString newList = InLabel.ToString();
		FString temp;
		int32 pos;
		newList.TrimStartInline();
		newList.TrimEndInline();
		HammUErSettings::reducedImportEntityList.Empty();
		while (newList.Len() > 0)
		{
			pos = newList.Find(",");
			if (pos == -1)
				pos = newList.Len();
			temp = newList.Mid(0, pos);
			temp.TrimEndInline();
			int32 found = FCString::Atoi(*temp);
			HammUErSettings::reducedImportEntityList.Add(found);
			newList.RemoveAt(0, pos + 1);
		}
	}
}

FText SHammerSettingsWidget::GetImportEntityList() const
{
	FString result = "";
	for (int i = 0; i < HammUErSettings::reducedImportEntityList.Num(); i++)
		result += (FString::FromInt(HammUErSettings::reducedImportEntityList[i]) + (i == HammUErSettings::reducedImportEntityList.Num() - 1 ? "" : ","));
	return FText::FromString(result);
}


#undef LOCTEXT_NAMESPACE