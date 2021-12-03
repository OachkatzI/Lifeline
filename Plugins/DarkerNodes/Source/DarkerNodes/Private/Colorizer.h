﻿/* Copyright (C) 2021 Hugo ATTAL - All Rights Reserved
* This plugin is downloadable from the Unreal Engine Marketplace
*/

#pragma once

#include "CoreMinimal.h"

#include "DarkerNodesSettings.h"
#include "Lib/BrushDatabase.h"
#include "Styling/SlateStyle.h"

class FColorizer
{
public:
	FColorizer();
	void Color();

private:
	void ReloadStyle();
	void SaveStyleForUMG();
	void ApplyFonts() const;

	// Colorizers
	void ColorText();
	void ColorGraph();
	void ColorButtons();
	void ColorCheckbox();
	void ColorPanel();
	void ColorWindow();
	void ColorIcons();
	void ColorIconsCustom();

	static void ReloadTextureResources();

	// Inside Var
	FString PluginDirectory;
	FString ThemeDirectory;
	FString FontsDirectory;
	FString GlobalSettingsFile;
	FSlateStyleSet* EditorStyle;
	FSlateStyleSet* CoreStyle;
	FSlateStyleSet* NiagaraStyle;

	const FString CurrentPluginVersion = "2.6";

	// Utilities
	void LoadDatabase();
	void ApplyParameters() const;

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 25
	void Reload(FName PropertyName);
#else
	void Reload(UObject* Object, struct FPropertyChangedEvent& Property);
#endif
	
	
	FColor MixColor(FColor Base, FColor Layer, float Alpha) const;
	FColor OffsetColor(FColor Base, int Offset);

	void AddMenuIcon(FSlateStyleSet* StyleSet, FString Name, FString Location, FColor Color) const;
	void ApplyImageBrush(FSlateStyleSet* StyleSet, FName Name, FString Location, FVector2D Size) const;
	void ApplyImageBrush(FSlateStyleSet* StyleSet, FName Name, FString Location, FVector2D Size, FColor Color) const;
	
	FSlateImageBrush ImageBrush(FString Location, FVector2D Size = FVector2D(16, 16)) const;
	FSlateImageBrush ImageBrush(FString Location, FVector2D Size, FColor Color) const;
	FSlateImageBrush ColorImageBrush(FColor Color = FColor::White, FVector2D Size = FVector2D(16, 16)) const;

	void ApplyBoxBrush(FSlateStyleSet* StyleSet, FName Name, FString Location, FMargin Margin = 0.25, FColor Color = FColor::White) const;
	void ApplyColorBoxBrush(FSlateStyleSet* StyleSet, FName Name, FColor Color) const;
	FSlateBoxBrush BoxBrush(FString Location, FVector2D Size, FMargin Margin = 0.25, FColor Color = FColor::White) const;

	void ApplyColorBorderBrush(FSlateStyleSet* StyleSet, FName Name, FColor Color) const;
	FSlateBorderBrush BorderBrush(FString Location, FMargin Margin = FMargin(0.25), FColor Color = FColor::White) const;
	FSlateBorderBrush ColorBorderBrush(FColor Color, FMargin Margin = FMargin(0.25)) const;

	void ApplyCenterIcon(FSlateStyleSet* StyleSet, FName Name, FString Location, FVector2D Size = FVector2D(16, 16), FColor Color = FColor::White) const;
	FSlateBrush* CenterIcon(FName Name, FString Location, FVector2D Size = FVector2D(16, 16), FColor Color = FColor::White) const;
	
	void ResetColors();
	FColor InvertLight(FColor Color);
	FSlateBrush* SlateBrush(FString Name) const;

	int32 GetFontSize(int32 BaseSize) const;

	// Icons

	FVector2D Icon0 = FVector2D::ZeroVector;
	FVector2D Icon8 = FVector2D(8, 8);
	FVector2D Icon10 = FVector2D(10, 10);
	FVector2D Icon12 = FVector2D(12, 12);
	FVector2D Icon14 = FVector2D(14, 14);
	FVector2D Icon16 = FVector2D(16, 16);
	FVector2D Icon20 = FVector2D(20, 20);
	FVector2D Icon24 = FVector2D(24, 24);
	FVector2D Icon32 = FVector2D(32, 32);
	FVector2D Icon40 = FVector2D(40, 40);
	FVector2D Icon256 = FVector2D(256, 256);

	// Colors

	FColor DebugRed = FColor(255, 0, 0);
	FColor DebugGreen = FColor(0, 255, 0);
	FColor DebugBlue = FColor(0, 0, 255);

	FColor GreyDark = FColor(20, 20, 20);
	FColor GreyBase = FColor(40, 40, 40);
	FColor GreyLight = FColor(120, 120, 120);

	FColor Primary;
	FColor HoverDark;
	FColor HoverBase;
	FColor HoverBaseBright;

	FColor TextColor = FColor(200, 200, 200);
	FColor TextShadow = FColor(10, 10, 10);
	FColor ScrollbarColor = FColor(20, 20, 20);
	FColor MainWindowColor = FColor(40, 40, 40);
	FColor ChildWindowColor = FColor(40, 40, 40);

	FColor GridLineColor;
	FColor GridRuleColor;
	FColor GridCenterColor;

	FColor RegularNodeBackground;
	FColor RegularNodeBorder;
	FColor VarNodeBackground;
	FColor VarNodeBorder;

	FColor ButtonBorderColor = FColor::Transparent;
	FColor ButtonPrimary = FColor(0, 96, 178);
	FColor ButtonSuccess = FColor(58, 161, 17);
	FColor ButtonInfo = FColor(0, 96, 178);
	FColor ButtonWarning = FColor(223, 179, 0);
	FColor ButtonDanger = FColor(178, 0, 0);

	FColor ImageColor = FColor(255,255,255);

	FTextBlockStyle NormalText;
	
	UBrushDatabase* BrushDatabase;
	UDarkerNodesSettings* DarkerNodesSettings;
};
