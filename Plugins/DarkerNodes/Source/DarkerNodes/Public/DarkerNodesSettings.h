/* Copyright (C) 2021 Hugo ATTAL - All Rights Reserved
* This plugin is downloadable from the Unreal Engine Marketplace
*/

#pragma once

#include "Engine/DeveloperSettings.h"
#include "DarkerNodesSettings.generated.h"

UENUM(BlueprintType)
enum class EHeaderStyle : uint8
{
	Gradient,
	Plain,
	Outline
};

UENUM(BlueprintType)
enum class EThemeLight : uint8
{
	Dark,
	Darker,
	Darkest
};

UENUM(BlueprintType)
enum class EThemeTemperature : uint8
{
	Cooler,
	Cool,
	Normal,
	Warm,
	Warmer
};

UENUM(BlueprintType)
enum class EPrimaryColor : uint8
{
	White,
	Orange,
	Cyan,
	Red,
	Purple,
	Green
};

UENUM(BlueprintType)
enum class EButtonPadding : uint8
{
	None,
	Small,
	Normal,
	Big
};

UENUM(BlueprintType)
enum class EButtonBorder : uint8
{
	None,
	Dark,
	Light
};

UENUM(BlueprintType)
enum class EFontFamily : uint8
{
	BalsamiqSans UMETA(DisplayName = "BalsamiqSans (Handwritten)"),
	Cannonade UMETA(DisplayName = "Cannonade (Sans Serif)"),
	CaskaydiaCove UMETA(DisplayName = "Caskaydia Cove (Monospace)"),
	EudoxusSans UMETA(DisplayName = "Exodus Sans (Sans Serif)"),
	GolosUI UMETA(DisplayName = "Golos UI (Sans Serif)"),
	Jua UMETA(DisplayName = "Jua (Handwritten)"),
	Junction UMETA(DisplayName = "Junction (Sans Serif)"),
	NewTelegraph UMETA(DisplayName = "New Telegraph (Slab Serif)"),
	Roboto UMETA(DisplayName = "Roboto (Sans Serif)"),
	XXIIAven UMETA(DisplayName = "XXII Aven (Sans Serif)")
};

UENUM(BlueprintType)
enum class EFontSize : uint8
{
	Small,
	Normal,
	Big,
	Bigger
};

UENUM(BlueprintType)
enum class EBlueprintVarNodeLine : uint8
{
	Thin,
	Thick
};

UENUM(BlueprintType)
enum class EBlueprintVarNodeStyle : uint8
{
	DarkSolid,
	LightSolid,
	DarkGlass,
	LightGlass
};

UENUM(BlueprintType)
enum class EIconStyle : uint8
{
	Line,
	Solid
};


UCLASS(config = EditorPerProjectUserSettings, meta = (DisplayName = "Darker Nodes Plugin"))
class DARKERNODES_API UDarkerNodesSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UDarkerNodesSettings()
	{
		CategoryName = TEXT("Plugins");
		SectionName = TEXT("Darker Nodes Plugin");
	}

	/* -----[ Activation ] ----- */

	/* Activate or deactivate the whole plugin. Default: true */
	UPROPERTY(config, EditAnywhere, Category = "Activation")
	bool MasterActivate = true;

	/* Use global settings across all your projects. When activated, it will load the global settings (overwriting this one).
	If no global settings exists, it will create it based on this one. Future updates will then be saved to global settings. */
	UPROPERTY(config, EditAnywhere, Category = "Activation")
	bool UseGlobalSettings = false;

	/* Force reload the global settings (if it was modified outside this instance for example). */
	UPROPERTY(config, EditAnywhere, Category = "Activation", meta = (EditCondition = "UseGlobalSettings"))
	bool LoadGlobalSettings = false;

	/* Reload the default style (you must disable Master Activate first). */
	UPROPERTY(config, EditAnywhere, Category = "Activation", meta = (EditCondition = "!MasterActivate"))
	bool ReloadDefaultStyle = false;

	/* This should fix missing elements by updating and saving the theme materials. */
	UPROPERTY(config, EditAnywhere, Category = "Activation", meta = (DisplayName = "Fix Missing Elements"))
	bool UpdateMaterials = true;

	/* Internal value to fix elements on plugin update. */
	UPROPERTY(config)
	FString PluginVersionUpdate = "";

	/* -----[ Theme ] ----- */

	/* Variations of the grey background (making it lighter or darker). Default: darker */
	UPROPERTY(config, EditAnywhere, Category = "Theme")
	EThemeLight ThemeLight = EThemeLight::Darker;

	/* Use light theme and invert luminosity (warning: experimental feature) */
	UPROPERTY(config, EditAnywhere, Category = "Theme", meta = (DisplayName = "Light Theme (experimental)"))
	bool LightTheme = false;

	/* Variations of the grey background (making it more orange or cyan). Default: normal */
	UPROPERTY(config, EditAnywhere, Category = "Theme")
	EThemeTemperature ThemeTemperature = EThemeTemperature::Normal;

	/* Variations of the primary color. Default: orange */
	UPROPERTY(config, EditAnywhere, Category = "Theme")
	EPrimaryColor PrimaryColor = EPrimaryColor::Orange;

	/* -----[ Buttons / Text fields ] ----- */

	/* Padding of buttons and text fields. Default: normal */
	UPROPERTY(config, EditAnywhere, Category = "ButtonsAndTextFields")
	EButtonPadding ButtonPadding = EButtonPadding::Normal;

	/* Radius of buttons and text fields. Default: 2px */
	UPROPERTY(config, EditAnywhere, Category = "ButtonsAndTextFields", meta = (ClampMin = "0"))
	int32 ButtonRadius = 2;

	/* Add a border to buttons and text fields. Default: None */
	UPROPERTY(config, EditAnywhere, Category = "ButtonsAndTextFields")
	EButtonBorder ButtonBorder = EButtonBorder::None;

	/* -----[ Fonts ] ----- */

	/* Font to use in the editor (it won't work on all texts). Default: Normal */
	UPROPERTY(config, EditAnywhere, Category = "Fonts", meta = (DisplayName = "Font Size (experimental)"))
	EFontSize FontSize = EFontSize::Normal;

	/* Font to use in the editor. Default: Roboto */
	UPROPERTY(config, EditAnywhere, Category = "Fonts")
	EFontFamily FontFamily = EFontFamily::Roboto;

	/* Use a custom font for regular texts. Default: false */
	UPROPERTY(config, EditAnywhere, Category = "Fonts")
	bool UseCustomRegularFont = false;

	/* Path to the regular TTF font. */
	UPROPERTY(config, EditAnywhere, Category = "Fonts", meta = (EditCondition = "UseCustomRegularFont", FilePathFilter="ttf"))
	FFilePath RegularFont;

	/* Use a custom font for bold texts. Default: false */
	UPROPERTY(config, EditAnywhere, Category = "Fonts")
	bool UseCustomBoldFont = false;

	/* Path to the bold TTF font. */
	UPROPERTY(config, EditAnywhere, Category = "Fonts", meta = (EditCondition = "UseCustomBoldFont", FilePathFilter="ttf"))
	FFilePath BoldFont;

	/* Use a custom font for italic texts. Default: false */
	UPROPERTY(config, EditAnywhere, Category = "Fonts")
	bool UseCustomItalicFont = false;

	/* Path to the italic TTF font. */
	UPROPERTY(config, EditAnywhere, Category = "Fonts", meta = (EditCondition = "UseCustomItalicFont", FilePathFilter="ttf"))
	FFilePath ItalicFont;

	/* -----[ Blueprint ] ----- */

	/* Activate or deactivate the blueprint theme. Default: true */
	UPROPERTY(config, EditAnywhere, Category = "Blueprint")
	bool ActivateBlueprintTheme = true;

	/* Radius of regular nodes. Default: 2px */
	UPROPERTY(config, EditAnywhere, Category = "Blueprint", meta = (EditCondition = "ActivateBlueprintTheme", ClampMin = "0"))
	int32 BlueprintRegularNodeRadius = 2;

	/* Radius of var nodes. Default: 2px */
	UPROPERTY(config, EditAnywhere, Category = "Blueprint", meta = (EditCondition = "ActivateBlueprintTheme", ClampMin = "0"))
	int32 BlueprintVarNodeRadius = 2;

	/* Style of the VarNode top line. Default: Thin */
	UPROPERTY(config, EditAnywhere, Category = "Blueprint", meta = (EditCondition = "ActivateBlueprintTheme"))
	EBlueprintVarNodeLine BlueprintVarNodeLine = EBlueprintVarNodeLine::Thin;

	/* Style of the VarNode background. Default: Dark Glass */
	UPROPERTY(config, EditAnywhere, Category = "Blueprint", meta = (EditCondition = "ActivateBlueprintTheme"))
	EBlueprintVarNodeStyle BlueprintVarNodeStyle = EBlueprintVarNodeStyle::DarkGlass;

	/* Opacity of blueprint nodes header. Default: 0.25 */
	UPROPERTY(config, EditAnywhere, Category = "Blueprint", meta = (ClampMin = "0", ClampMax="1"))
	float BlueprintNodeHeaderOpacity = 0.25f;

	/* Saturation of blueprint nodes header. Default: 1.0 */
	UPROPERTY(config, EditAnywhere, Category = "Blueprint", meta = (ClampMin = "0", ClampMax="1"))
	float BlueprintNodeHeaderSaturation = 1.0f;

	/* Add a padding to make nodes match the grid on the top left corner. Default: false */
	UPROPERTY(config, EditAnywhere, Category = "Blueprint")
	bool ExtendNodes = false;

	/* Disable the blueprint grid. Default: false */
	UPROPERTY(config, EditAnywhere, Category = "Blueprint")
	bool DisableBlueprintGrid = false;

	/* Disable the UMG grid. Default: false */
	UPROPERTY(config, EditAnywhere, Category = "Blueprint", meta = (EditCondition = "DisableBlueprintGrid"))
	bool DisableUMGGrid = false;

	/* Opacity of the origin axis of the background grid. Default: 0.5 */
	UPROPERTY(config, EditAnywhere, Category = "Blueprint", meta = (ClampMin = "0", ClampMax="1"))
	float OriginAxisOpacity = 0.5f;

	/* -----[ Miscellaneous ] ----- */

	/* Use the dark scrollbar instead of the white one. Default: true */
	UPROPERTY(config, EditAnywhere, Category = "Miscellaneous")
	bool UseCustomScrollbar = true;

	/* Use the custom icons of Darker Nodes. Default: true */
	UPROPERTY(config, EditAnywhere, Category = "Miscellaneous")
	bool UseCustomIcons = true;

	/* Change the style of the custom icons. Default: Line */
	UPROPERTY(config, EditAnywhere, Category = "Miscellaneous", meta = (EditCondition = "UseCustomIcons"))
	EIconStyle IconStyle = EIconStyle::Line;

	/* Make icons colored by group. Default: true */
	UPROPERTY(config, EditAnywhere, Category = "Miscellaneous", meta = (EditCondition = "UseCustomIcons"))
	bool UseIconColorization = false;

	/* Use the custom panels of Darker Nodes. Default: true */
	UPROPERTY(config, EditAnywhere, Category = "Miscellaneous")
	bool UseCustomPanels = true;

	/* Use the custom buttons, text fields and checkboxes of Darker Nodes. Default: true */
	UPROPERTY(config, EditAnywhere, Category = "Miscellaneous")
	bool UseCustomButton = true;

	/* -----[ Customization ] ----- */

	/* When "Use [Type] Customization" is unticked, overwrite the customized colors with the current theme colors. Default: true */
	UPROPERTY(config, EditAnywhere, Category = "Customization")
	bool OverwriteColors = true;

	/* Customize the backgrounds more in-depth. Default: false */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Background")
	bool UseGreyCustomization = false;

	/* Dark background of the theme. Default: RGB(20,20,20) */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Background", meta = (EditCondition = "UseGreyCustomization"))
	FColor GreyDark = FColor(20, 20, 20);

	/* Base background of the theme. Default: RGB(40,40,40) */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Background", meta = (EditCondition = "UseGreyCustomization"))
	FColor GreyBase = FColor(40, 40, 40);

	/* Light background of the theme. Default: RGB(80,80,80) */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Background", meta = (EditCondition = "UseGreyCustomization"))
	FColor GreyLight = FColor(80, 80, 80);

	/* Customize the colors more in-depth. Default: false */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Color")
	bool UseColorCustomization = false;

	/* Hover color on dark background. Default: RGB(229,110,23) */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Color", meta = (EditCondition = "UseColorCustomization"))
	FColor CustomPrimaryColor = FColor(229, 110, 23);

	/* Text color of the theme. Default: RGB(200,200,200) */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Color", meta = (EditCondition = "UseColorCustomization"))
	FColor TextColor = FColor(200, 200, 200);

	/* Color of the scrollbar. Default: RGB(20,20,20) */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Color", meta = (EditCondition = "UseColorCustomization"))
	FColor ScrollbarColor = FColor(20, 20, 20);

	/* Customize the colors more in-depth. Default: false */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Windows")
	bool UseWindowCustomization = false;

	/* Outline color of the main editor window. Default: RGB(40,40,40) */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Windows", meta = (EditCondition = "UseWindowCustomization"))
	FColor MainWindowColor = FColor(40, 40, 40);

	/* Outline color of child editor windows. Default: RGB(40,40,40) */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Windows", meta = (EditCondition = "UseWindowCustomization"))
	FColor ChildWindowColor = FColor(40, 40, 40);

	/* Customize the colors of Blueprint graphs. Default: false */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Blueprint")
	bool UseBlueprintColorCustomization = false;

	/* Color for grid lines in Blueprint graphs and UMG editor. */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Blueprint", meta = (EditCondition = "UseBlueprintColorCustomization"))
	FColor GridLineColor;

	/* Color for grid rules in Blueprint graphs and UMG editor. */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Blueprint", meta = (EditCondition = "UseBlueprintColorCustomization"))
	FColor GridRuleColor;

	/* Color for grid center in Blueprint graphs and UMG editor. */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Blueprint", meta = (EditCondition = "UseBlueprintColorCustomization"))
	FColor GridCenterColor;

	/* Background of regular Blueprint nodes. */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Blueprint", meta = (EditCondition = "UseBlueprintColorCustomization"))
	FColor RegularNodeBackground;

	/* Border of regular Blueprint nodes. */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Blueprint", meta = (EditCondition = "UseBlueprintColorCustomization"))
	FColor RegularNodeBorder;

	/* Background of var Blueprint nodes. */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Blueprint", meta = (EditCondition = "UseBlueprintColorCustomization"))
	FColor VarNodeBackground;

	/* Border of var Blueprint nodes. */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Blueprint", meta = (EditCondition = "UseBlueprintColorCustomization"))
	FColor VarNodeBorder;

	/* Customize the colors more in-depth. Default: false */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Buttons")
	bool UseButtonColorCustomization = false;

	/* Color for buttons border. Default: RGB(20,20,20) */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Buttons", meta = (EditCondition = "UseButtonColorCustomization"))
	FColor ButtonBorderColor = FColor(20, 20, 20);

	/* Color for primary buttons. Default: RGB(0,96,178) */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Buttons", meta = (EditCondition = "UseButtonColorCustomization"))
	FColor ButtonPrimary = FColor(0, 96, 178);

	/* Color for success buttons. Default: RGB(58,161,17) */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Buttons", meta = (EditCondition = "UseButtonColorCustomization"))
	FColor ButtonSuccess = FColor(58, 161, 17);

	/* Color for info buttons. Default: RGB(0,96,178) */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Buttons", meta = (EditCondition = "UseButtonColorCustomization"))
	FColor ButtonInfo = FColor(0, 96, 178);

	/* Color for warning buttons. Default: RGB(223,179,0) */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Buttons", meta = (EditCondition = "UseButtonColorCustomization"))
	FColor ButtonWarning = FColor(223, 179, 0);

	/* Color for danger buttons. Default: RGB(178,0,0) */
	UPROPERTY(config, EditAnywhere, Category = "Customization|Buttons", meta = (EditCondition = "UseButtonColorCustomization"))
	FColor ButtonDanger = FColor(178, 0, 0);

	/* Force texture resources reload. */
	UPROPERTY(config, EditAnywhere, Category = "Debug")
	bool ReloadTextureResources = false;

	virtual FName GetContainerName() const override
	{
		return "Editor";
	}

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 25
	DECLARE_EVENT_OneParam(UDarkerNodesSettings, FSettingChangedEvent, FName);
	FSettingChangedEvent& OnSettingChanged( ) { return SettingChangedEvent; }
    
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);
    
		const FName Name = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
		SettingChangedEvent.Broadcast(Name);
	}
    
	private:
    
	FSettingChangedEvent SettingChangedEvent;
#endif
};
