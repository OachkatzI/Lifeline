/* Copyright (C) 2021 Hugo ATTAL - All Rights Reserved
* This plugin is downloadable from the Unreal Engine Marketplace
*/

#include "Colorizer.h"

#include "EditorStyleSet.h"
#include "Interfaces/IPluginManager.h"
#include "Classes/EditorStyleSettings.h"
#include "SettingsEditor/Public/ISettingsEditorModule.h"
#include "NiagaraEditorStyle.h"


FColorizer::FColorizer()
{
	LoadDatabase();

	DarkerNodesSettings = GetMutableDefault<UDarkerNodesSettings>();

	DarkerNodesSettings->OnSettingChanged().AddRaw(this, &FColorizer::Reload);
	FCoreDelegates::OnPostEngineInit.AddLambda([this]()
	{
		if (DarkerNodesSettings->UpdateMaterials || DarkerNodesSettings->PluginVersionUpdate != CurrentPluginVersion)
		{
			DarkerNodesSettings->UpdateMaterials = false;
			DarkerNodesSettings->PluginVersionUpdate = CurrentPluginVersion;
			DarkerNodesSettings->SaveConfig();
			BrushDatabase->UpdateAndSaveMaterials();
		}
	});

	EditorStyle = static_cast<FSlateStyleSet*>(&FEditorStyle::Get());
	CoreStyle = static_cast<FSlateStyleSet*>(const_cast<ISlateStyle*>(&FCoreStyle::Get()));
	NiagaraStyle = static_cast<FSlateStyleSet*>(const_cast<ISlateStyle*>(&FNiagaraEditorStyle::Get()));
	PluginDirectory = IPluginManager::Get().FindPlugin(TEXT("DarkerNodes"))->GetBaseDir();
	ThemeDirectory = PluginDirectory + FString("/Resources/Theme");
	FontsDirectory = PluginDirectory + FString("/Resources/Fonts");
	GlobalSettingsFile = PluginDirectory + "/Settings.ini";

	if (DarkerNodesSettings->UseGlobalSettings)
	{
		if (FPaths::FileExists(GlobalSettingsFile))
		{
			DarkerNodesSettings->LoadConfig(nullptr, *GlobalSettingsFile);
		}
	}

	ReloadStyle();
}

void FColorizer::LoadDatabase()
{
	BrushDatabase = NewObject<UBrushDatabase>();

	BrushDatabase->CreateSlateBrush("GreyBase", "SolidColor.SolidColor");
	BrushDatabase->CreateSlateBrush("GreyDark", "SolidColor.SolidColor");
	BrushDatabase->CreateSlateBrush("GreyLight", "SolidColor.SolidColor");
	BrushDatabase->CreateSlateBrush("Debug", "SolidColor.SolidColor");

	BrushDatabase->CreateSlateBrush("HoverDark", "SolidColor.SolidColor");
	BrushDatabase->CreateSlateBrush("HoverBase", "SolidColor.SolidColor");
	BrushDatabase->CreateSlateBrush("HoverBaseBright", "SolidColor.SolidColor");

	BrushDatabase->CreateSlateBrush("Primary", "SolidColor.SolidColor");
	BrushDatabase->CreateSlateColor("TextColor");
	BrushDatabase->CreateSlateBrush("ScrollbarColor", "SolidColor.SolidColor");
	BrushDatabase->CreateSlateBrush("MainWindowColor", "SolidColor.SolidColor");
	BrushDatabase->CreateSlateBrush("ChildWindowColor", "SolidColor.SolidColor");

	BrushDatabase->CreateSlateColor("GridLine");
	BrushDatabase->CreateSlateColor("GridRule");
	BrushDatabase->CreateSlateColor("GridCenter");

	BrushDatabase->CreateSlateBrush("Button", "Button.Button");
	BrushDatabase->CreateSlateBrush("Button_Hovered", "Button.Button");
	BrushDatabase->CreateSlateBrush("Button_Pressed", "Button.Button");
	BrushDatabase->CreateSlateBrush("Button_Disabled", "Button.Button");

	BrushDatabase->CreateSlateBrush("Button_Start", "ButtonCut.ButtonCut");
	BrushDatabase->CreateSlateBrush("Button_Start_Hovered", "ButtonCut.ButtonCut");
	BrushDatabase->CreateSlateBrush("Button_Start_Pressed", "ButtonCut.ButtonCut");
	BrushDatabase->CreateSlateBrush("Button_Start_Checked", "ButtonCut.ButtonCut");
	BrushDatabase->CreateSlateBrush("Button_Start_Hovered_Checked", "ButtonCut.ButtonCut");

	BrushDatabase->CreateSlateBrush("Button_Middle", "ButtonCut.ButtonCut");
	BrushDatabase->CreateSlateBrush("Button_Middle_Hovered", "ButtonCut.ButtonCut");
	BrushDatabase->CreateSlateBrush("Button_Middle_Pressed", "ButtonCut.ButtonCut");
	BrushDatabase->CreateSlateBrush("Button_Middle_Checked", "ButtonCut.ButtonCut");
	BrushDatabase->CreateSlateBrush("Button_Middle_Hovered_Checked", "ButtonCut.ButtonCut");

	BrushDatabase->CreateSlateBrush("Button_End", "ButtonCut.ButtonCut");
	BrushDatabase->CreateSlateBrush("Button_End_Hovered", "ButtonCut.ButtonCut");
	BrushDatabase->CreateSlateBrush("Button_End_Pressed", "ButtonCut.ButtonCut");
	BrushDatabase->CreateSlateBrush("Button_End_Checked", "ButtonCut.ButtonCut");
	BrushDatabase->CreateSlateBrush("Button_End_Hovered_Checked", "ButtonCut.ButtonCut");

	BrushDatabase->CreateSlateBrush("RegularNode_body", "Box.Box");
	BrushDatabase->CreateSlateBrush("VarNode_body", "Box.Box");
	BrushDatabase->CreateSlateBrush("RegularNode_shadow_selected", "Box.Box");
	BrushDatabase->CreateSlateBrush("VarNode_shadow_selected", "Box.Box");
	BrushDatabase->CreateSlateBrush("RegularNode_color_spill", "HeaderBox.HeaderBox");

	BrushDatabase->CreateSlateBrush("PanelDark", "Panel.Panel");
}

void FColorizer::ResetColors()
{
	GreyDark = FColor(20, 20, 20);
	GreyBase = FColor(40, 40, 40);
	GreyLight = FColor(120, 120, 120);

	TextColor = FColor(200, 200, 200);
	TextShadow = FColor(10, 10, 10);

	ButtonPrimary = FColor(0, 96, 178);
	ButtonSuccess = FColor(58, 161, 17);
	ButtonInfo = FColor(0, 96, 178);
	ButtonWarning = FColor(223, 179, 0);
	ButtonDanger = FColor(178, 0, 0);
}

void FColorizer::ReloadStyle()
{
	if (!DarkerNodesSettings->MasterActivate)
	{
		return;
	}

	ResetColors();

	switch (DarkerNodesSettings->ThemeLight)
	{
	case EThemeLight::Dark:
		GreyBase = OffsetColor(GreyBase, 20);
		GreyDark = OffsetColor(GreyDark, 20);
		GreyLight = OffsetColor(GreyLight, 20);
		break;
	case EThemeLight::Darker:
		break;
	case EThemeLight::Darkest:
		GreyBase = OffsetColor(GreyBase, -20);
		GreyDark = OffsetColor(GreyDark, -10);
		GreyLight = OffsetColor(GreyLight, -10);
		break;
	}

	if (DarkerNodesSettings->LightTheme)
	{
		GreyBase = InvertLight(GreyBase);
		GreyDark = InvertLight(GreyDark);
		GreyLight = InvertLight(GreyLight);
		TextColor = InvertLight(TextColor);
		TextShadow = InvertLight(TextShadow);
		ImageColor = InvertLight(ImageColor);
	}

	const FColor WarmLayer = FColor(229, 110, 23);
	const FColor CoolLayer = FColor(23, 141, 229);

	switch (DarkerNodesSettings->ThemeTemperature)
	{
	case EThemeTemperature::Cooler:
		GreyBase = MixColor(GreyBase, CoolLayer, 0.05);
		GreyDark = MixColor(GreyDark, CoolLayer, 0.05);
		GreyLight = MixColor(GreyLight, CoolLayer, 0.05);
		break;
	case EThemeTemperature::Cool:
		GreyBase = MixColor(GreyBase, CoolLayer, 0.025);
		GreyDark = MixColor(GreyDark, CoolLayer, 0.025);
		GreyLight = MixColor(GreyLight, CoolLayer, 0.025);
		break;
	case EThemeTemperature::Normal:
		break;
	case EThemeTemperature::Warm:
		GreyBase = MixColor(GreyBase, WarmLayer, 0.025);
		GreyDark = MixColor(GreyDark, WarmLayer, 0.025);
		GreyLight = MixColor(GreyLight, WarmLayer, 0.025);
		break;
	case EThemeTemperature::Warmer:
		GreyBase = MixColor(GreyBase, WarmLayer, 0.05);
		GreyDark = MixColor(GreyDark, WarmLayer, 0.05);
		GreyLight = MixColor(GreyLight, WarmLayer, 0.05);
		break;
	}

	// Cyan = Orange + 180
	// Purple = Orange - 100
	// Green = Orange + 80
	// Red = Orange - 25

	const FColor White = FColor(126, 126, 126);
	const FColor Orange = FColor(229, 110, 23);
	const FColor Cyan = FColor(23, 141, 229);
	const FColor Red = FColor(229, 25, 23);
	const FColor Purple = FColor(178, 23, 229);
	const FColor Green = FColor(74, 229, 23);

	switch (DarkerNodesSettings->PrimaryColor)
	{
	case EPrimaryColor::White:
		Primary = White;
		break;
	case EPrimaryColor::Orange:
		Primary = Orange;
		break;
	case EPrimaryColor::Cyan:
		Primary = Cyan;
		break;
	case EPrimaryColor::Red:
		Primary = Red;
		break;
	case EPrimaryColor::Purple:
		Primary = Purple;
		break;
	case EPrimaryColor::Green:
		Primary = Green;
		break;
	}

	if (DarkerNodesSettings->UseGreyCustomization)
	{
		GreyBase = DarkerNodesSettings->GreyBase;
		GreyDark = DarkerNodesSettings->GreyDark;
		GreyLight = DarkerNodesSettings->GreyLight;
	}

	ScrollbarColor = GreyDark;
	MainWindowColor = GreyBase;
	ChildWindowColor = GreyBase;

	RegularNodeBackground = MixColor(FColor::Black, GreyDark, 0.5f).WithAlpha(255 * 0.75f);
	RegularNodeBorder = MixColor(FColor::Black, GreyDark, 0.5f);

	switch (DarkerNodesSettings->BlueprintVarNodeStyle)
	{
	case EBlueprintVarNodeStyle::DarkSolid:
		VarNodeBackground = MixColor(FColor::Black, GreyDark, 0.5f).WithAlpha(255 * 0.75f);
		VarNodeBorder = MixColor(FColor::Black, GreyDark, 0.5f);
		break;
	case EBlueprintVarNodeStyle::LightSolid:
		VarNodeBackground = GreyLight.WithAlpha(255 * 0.30f);
		VarNodeBorder = GreyLight;
		break;
	case EBlueprintVarNodeStyle::DarkGlass:
		VarNodeBackground = MixColor(FColor::Black, GreyDark, 0.5f).WithAlpha(255 * 0.30f);
		VarNodeBorder = MixColor(FColor::Black, GreyDark, 0.5f);
		break;
	case EBlueprintVarNodeStyle::LightGlass:
		VarNodeBackground = GreyLight.WithAlpha(255 * 0.10f);
		VarNodeBorder = GreyLight;
		break;
	}

	switch (DarkerNodesSettings->ButtonBorder)
	{
	case EButtonBorder::None:
		ButtonBorderColor = GreyDark;
		break;
	case EButtonBorder::Dark:
		ButtonBorderColor = MixColor(GreyDark, FColor::Black, 0.5f);
		break;
	case EButtonBorder::Light:
		ButtonBorderColor = MixColor(GreyDark, FColor::White, 0.1f);
		break;
	}

	if (!DarkerNodesSettings->DisableUMGGrid)
	{
		GridLineColor = MixColor(GreyDark, GreyBase, 0.5f);
		GridRuleColor = GreyBase;
		GridCenterColor = MixColor(GreyBase, GreyLight, DarkerNodesSettings->OriginAxisOpacity);
	}
	else
	{
		GridLineColor = GreyDark;
		GridRuleColor = GreyDark;
		GridCenterColor = MixColor(GreyDark, GreyLight, DarkerNodesSettings->OriginAxisOpacity);
	}

	if (DarkerNodesSettings->UseColorCustomization)
	{
		Primary = DarkerNodesSettings->CustomPrimaryColor;
		TextColor = DarkerNodesSettings->TextColor;
		ScrollbarColor = DarkerNodesSettings->ScrollbarColor;
	}

	if (DarkerNodesSettings->UseWindowCustomization)
	{
		MainWindowColor = DarkerNodesSettings->MainWindowColor;
		ChildWindowColor = DarkerNodesSettings->ChildWindowColor;
	}

	if (DarkerNodesSettings->UseButtonColorCustomization)
	{
		ButtonBorderColor = DarkerNodesSettings->ButtonBorderColor;
		ButtonPrimary = DarkerNodesSettings->ButtonPrimary;
		ButtonSuccess = DarkerNodesSettings->ButtonSuccess;
		ButtonInfo = DarkerNodesSettings->ButtonInfo;
		ButtonWarning = DarkerNodesSettings->ButtonWarning;
		ButtonDanger = DarkerNodesSettings->ButtonDanger;
	}

	if (DarkerNodesSettings->UseBlueprintColorCustomization)
	{
		GridLineColor = DarkerNodesSettings->GridLineColor;
		GridRuleColor = DarkerNodesSettings->GridRuleColor;
		GridCenterColor = DarkerNodesSettings->GridCenterColor;

		RegularNodeBackground = DarkerNodesSettings->RegularNodeBackground;
		RegularNodeBorder = DarkerNodesSettings->RegularNodeBorder;
		VarNodeBackground = DarkerNodesSettings->VarNodeBackground;
		VarNodeBorder = DarkerNodesSettings->VarNodeBorder;
	}

	HoverDark = MixColor(GreyDark, Primary, 0.1);
	HoverBase = MixColor(GreyBase, Primary, 0.1);
	HoverBaseBright = MixColor(GreyBase, Primary, 0.5);

	if (DarkerNodesSettings->OverwriteColors)
	{
		if (!DarkerNodesSettings->UseGreyCustomization)
		{
			DarkerNodesSettings->GreyBase = GreyBase;
			DarkerNodesSettings->GreyDark = GreyDark;
			DarkerNodesSettings->GreyLight = GreyLight;
		}

		if (!DarkerNodesSettings->UseColorCustomization)
		{
			DarkerNodesSettings->CustomPrimaryColor = Primary;
			DarkerNodesSettings->TextColor = TextColor;
			DarkerNodesSettings->ScrollbarColor = ScrollbarColor;
		}

		if (!DarkerNodesSettings->UseWindowCustomization)
		{
			DarkerNodesSettings->MainWindowColor = MainWindowColor;
			DarkerNodesSettings->ChildWindowColor = ChildWindowColor;
		}

		if (!DarkerNodesSettings->UseButtonColorCustomization)
		{
			DarkerNodesSettings->ButtonBorderColor = ButtonBorderColor;
			DarkerNodesSettings->ButtonPrimary = ButtonPrimary;
			DarkerNodesSettings->ButtonSuccess = ButtonSuccess;
			DarkerNodesSettings->ButtonInfo = ButtonInfo;
			DarkerNodesSettings->ButtonWarning = ButtonWarning;
			DarkerNodesSettings->ButtonDanger = ButtonDanger;
		}

		if (!DarkerNodesSettings->UseBlueprintColorCustomization)
		{
			DarkerNodesSettings->GridLineColor = GridLineColor;
			DarkerNodesSettings->GridRuleColor = GridRuleColor;
			DarkerNodesSettings->GridCenterColor = GridCenterColor;

			DarkerNodesSettings->RegularNodeBackground = RegularNodeBackground;
			DarkerNodesSettings->RegularNodeBorder = RegularNodeBorder;
			DarkerNodesSettings->VarNodeBackground = VarNodeBackground;
			DarkerNodesSettings->VarNodeBorder = VarNodeBorder;
		}
	}

	ApplyParameters();
}

void FColorizer::Color()
{
	if (!DarkerNodesSettings->MasterActivate)
	{
		return;
	}

	if (!FApp::HasProjectName())
	{
		return;
	}

	SaveStyleForUMG();

	ColorText();
	ColorGraph();
	ColorButtons();
	ColorCheckbox();
	ColorPanel();
	ColorWindow();
	ColorIcons();
	ColorIconsCustom();

	ReloadTextureResources();
}


void FColorizer::ReloadTextureResources()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 25
void FColorizer::Reload(FName PropertyName)
#else
void FColorizer::Reload(UObject* Object, struct FPropertyChangedEvent& Property)
#endif
{
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION > 25
	const FName PropertyName = Property.GetPropertyName();
#endif

	if (DarkerNodesSettings->ReloadDefaultStyle)
	{
		UE_LOG(LogTemp, Log, TEXT("[Darker Nodes] Reloading default style..."));
		UEditorStyleSettings* StyleSettings = GetMutableDefault<UEditorStyleSettings>();
		StyleSettings->SelectionColor = FLinearColor(0.728f, 0.364f, 0.003f);
		StyleSettings->PressedSelectionColor = FLinearColor(0.701f, 0.225f, 0.003f);
		StyleSettings->InactiveSelectionColor = FLinearColor(0.25f, 0.25f, 0.25f);
		StyleSettings->EditorChildWindowBackgroundOverride = FSlateBrush();
		StyleSettings->EditorMainWindowBackgroundOverride = FSlateBrush();
		StyleSettings->RegularColor = FLinearColor(0.035, 0.035, 0.035);
		StyleSettings->RuleColor = FLinearColor(0.008, 0.008, 0.008);
		StyleSettings->CenterColor = FLinearColor::Black;
		StyleSettings->bUseGrid = true;
		StyleSettings->SaveConfig();
		UE_LOG(LogTemp, Log, TEXT("[Darker Nodes] Done!"));

		DarkerNodesSettings->ReloadDefaultStyle = false;
	}

	if (PropertyName == "UseGlobalSettings")
	{
		if (DarkerNodesSettings->UseGlobalSettings)
		{
			if (FPaths::FileExists(GlobalSettingsFile))
			{
				DarkerNodesSettings->LoadConfig(nullptr, *GlobalSettingsFile);
			}
			else
			{
				DarkerNodesSettings->SaveConfig(CPF_Config, *GlobalSettingsFile);
			}
		}
	}

	if (DarkerNodesSettings->LoadGlobalSettings)
	{
		if (FPaths::FileExists(GlobalSettingsFile))
		{
			DarkerNodesSettings->LoadConfig(nullptr, *GlobalSettingsFile);
		}
		DarkerNodesSettings->LoadGlobalSettings = false;
	}

	if (DarkerNodesSettings->ReloadTextureResources)
	{
		DarkerNodesSettings->ReloadTextureResources = false;
		ReloadTextureResources();
	}

	if (DarkerNodesSettings->UpdateMaterials)
	{
		DarkerNodesSettings->UpdateMaterials = false;
		BrushDatabase->UpdateAndSaveMaterials();
	}

	if (!DarkerNodesSettings->MasterActivate)
	{
		return;
	}

	ReloadStyle();

	ISettingsEditorModule* SettingsEditorModule = FModuleManager::GetModulePtr<ISettingsEditorModule>("SettingsEditor");
	if (SettingsEditorModule)
	{
		UEditorStyleSettings* StyleSettings = GetMutableDefault<UEditorStyleSettings>();
		StyleSettings->SelectionColor = FLinearColor::FromSRGBColor(Primary);
		StyleSettings->PressedSelectionColor = FLinearColor::FromSRGBColor(Primary);
		StyleSettings->InactiveSelectionColor = FLinearColor::FromSRGBColor(HoverBase);
		StyleSettings->bUseGrid = !DarkerNodesSettings->DisableBlueprintGrid;
		StyleSettings->SaveConfig();

		SettingsEditorModule->OnApplicationRestartRequired();
	}

	DarkerNodesSettings->SaveConfig();

	if (DarkerNodesSettings->UseGlobalSettings)
	{
		DarkerNodesSettings->SaveConfig(CPF_Config, *GlobalSettingsFile);
	}
}

FColor FColorizer::MixColor(FColor Base, FColor Layer, float Alpha) const
{
	Base.R = FMath::Lerp(Base.R, Layer.R, Alpha);
	Base.G = FMath::Lerp(Base.G, Layer.G, Alpha);
	Base.B = FMath::Lerp(Base.B, Layer.B, Alpha);
	return Base;
}

FColor FColorizer::OffsetColor(FColor Base, int Offset)
{
	Base.R += Offset;
	Base.G += Offset;
	Base.B += Offset;
	return Base;
}
