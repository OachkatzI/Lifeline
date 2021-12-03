/* Copyright (C) 2021 Hugo ATTAL - All Rights Reserved
* This plugin is downloadable from the Unreal Engine Marketplace
*/

#include "Colorizer.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PIEPreviewDeviceProfileSelector/Private/PIEPreviewWindowCoreStyle.h"

void FColorizer::ColorButtons()
{
	if (!DarkerNodesSettings->UseCustomButton)
	{
		return;
	}

	FMargin Margin;
	const FMargin SmallMargin = FMargin(2);

	switch (DarkerNodesSettings->ButtonPadding)
	{
	case EButtonPadding::None:
		Margin = FMargin(2, 0);
		break;
	case EButtonPadding::Small:
		Margin = FMargin(2);
		break;
	case EButtonPadding::Normal:
		Margin = FMargin(4);
		break;
	case EButtonPadding::Big:
		Margin = FMargin(8);
		break;
	}

	SlateBrush("Button")->SetImageSize(Icon16);
	SlateBrush("Button_Hovered")->SetImageSize(Icon16);
	SlateBrush("Button_Pressed")->SetImageSize(Icon16);

	// Button
	const FButtonStyle Button =
		FButtonStyle()
		.SetNormal(*SlateBrush("Button"))
		.SetHovered(*SlateBrush("Button_Hovered"))
		.SetPressed(*SlateBrush("Button_Pressed"))
		.SetDisabled(*SlateBrush("Button_Disabled"))
		.SetNormalPadding(Margin)
		.SetPressedPadding(Margin);
	CoreStyle->Set("Button", Button);

	const FComboButtonStyle ComboButton =
		FComboButtonStyle()
		.SetButtonStyle(Button)
		.SetDownArrowImage(ImageBrush("/Elements/Button/ComboArrow.png", Icon8))
		.SetMenuBorderBrush(*SlateBrush("Button"))
		.SetMenuBorderPadding(Margin);
	CoreStyle->Set("ComboButton", ComboButton);

	const FComboBoxStyle ComboBox = FComboBoxStyle()
		.SetComboButtonStyle(ComboButton);
	CoreStyle->Set("ComboBox", ComboBox);


	EditorStyle->Set("FilePath.FolderButton", Button);
	EditorStyle->Set("PropertyEditor.AssetComboStyle", Button);

	// Scrollbar

	FScrollBarStyle ScrollBar = CoreStyle->GetWidgetStyle<FScrollBarStyle>("ScrollBar");

	if (DarkerNodesSettings->UseCustomScrollbar)
	{
		ScrollBar =
			FScrollBarStyle()
			.SetVerticalTopSlotImage(ColorImageBrush(GreyBase, Icon8))
			.SetVerticalBottomSlotImage(ColorImageBrush(GreyBase, Icon8))
			.SetHorizontalTopSlotImage(ColorImageBrush(GreyBase, Icon8))
			.SetHorizontalBottomSlotImage(ColorImageBrush(GreyBase, Icon8))
			.SetNormalThumbImage(*SlateBrush("ScrollbarColor"))
			.SetDraggedThumbImage(*SlateBrush("ScrollbarColor"))
			.SetHoveredThumbImage(*SlateBrush("ScrollbarColor"));
	}

	CoreStyle->Set("Scrollbar", ScrollBar);
	EditorStyle->Set("Scrollbar", ScrollBar);


	// Textbox

	const FEditableTextBoxStyle NormalEditableTextBoxStyle =
		FEditableTextBoxStyle()
		.SetFont(NormalText.Font)
		.SetBackgroundImageNormal(*SlateBrush("Button"))
		.SetBackgroundImageHovered(*SlateBrush("Button_Hovered"))
		.SetBackgroundImageFocused(*SlateBrush("Button_Hovered"))
		.SetBackgroundImageReadOnly(*SlateBrush("Button_Disabled"))
		.SetScrollBarStyle(ScrollBar)
		.SetForegroundColor(*BrushDatabase->GetSlateColor("TextColor"));
	CoreStyle->Set("NormalEditableTextBox", NormalEditableTextBoxStyle);
	CoreStyle->Set("SpecialEditableTextBox", NormalEditableTextBoxStyle);
	NiagaraStyle->Set("NiagaraEditor.ParameterEditableTextBox", NormalEditableTextBoxStyle);

	EditorStyle->Set("EditableTextBox.Background.Normal", SlateBrush("Button"));
	EditorStyle->Set("EditableTextBox.Background.Hovered", SlateBrush("Button_Hovered"));
	EditorStyle->Set("EditableTextBox.Background.Focused", SlateBrush("Button_Hovered"));
	EditorStyle->Set("EditableTextBox.Background.ReadOnly", SlateBrush("Button_Disabled"));

	const FSpinBoxStyle SpinBox =
		FSpinBoxStyle()
		.SetBackgroundBrush(*SlateBrush("Button"))
		.SetHoveredBackgroundBrush(*SlateBrush("Button_Hovered"))
		.SetActiveFillBrush(*SlateBrush("Button_Hovered"))
		.SetInactiveFillBrush(*SlateBrush("Button"))
		.SetArrowsImage(ImageBrush("/Elements/Textbox/SpinArrows.png", Icon12))
		.SetForegroundColor(*BrushDatabase->GetSlateColor("TextColor"))
		.SetTextPadding(Margin);

	CoreStyle->Set("SpinBox", SpinBox);
	EditorStyle->Set("SpinBox", SpinBox);
	CoreStyle->Set("NumericEntrySpinBox", SpinBox);
	EditorStyle->Set("NumericEntrySpinBox", SpinBox);
	CoreStyle->Set("NumericEntrySpinBox_Dark", SpinBox);
	NiagaraStyle->Set("NiagaraEditor.ParameterSpinbox", SpinBox);

	// Tableview

	const FTableRowStyle DefaultTableRowStyle =
		FTableRowStyle()
		.SetEvenRowBackgroundBrush(FSlateNoResource())
		.SetEvenRowBackgroundHoveredBrush(ColorImageBrush(HoverBase))
		.SetOddRowBackgroundBrush(FSlateNoResource())
		.SetOddRowBackgroundHoveredBrush(ColorImageBrush(HoverBase))
		.SetSelectorFocusedBrush(BorderBrush("/Elements/Table/Selector.png", FMargin(0.25), HoverBaseBright))
		.SetActiveBrush(ColorImageBrush(HoverBase))
		.SetActiveHoveredBrush(ColorImageBrush(HoverBase))
		.SetInactiveBrush(ColorImageBrush(HoverBase))
		.SetInactiveHoveredBrush(ColorImageBrush(HoverBase))
		.SetActiveHighlightedBrush(ColorImageBrush(HoverBase))
		.SetInactiveHighlightedBrush(ColorImageBrush(HoverBase))
		.SetTextColor(FLinearColor::FromSRGBColor(TextColor))
		.SetSelectedTextColor(FLinearColor::FromSRGBColor(TextColor))
		.SetDropIndicator_Above(BorderBrush("/Elements/Table/DropZoneIndicator_Above.png", FMargin(10.0f / 16.0f, 10.0f / 16.0f, 0, 0), HoverBaseBright))
		.SetDropIndicator_Onto(BorderBrush("/Elements/Table/DropZoneIndicator_Onto.png", FMargin(0.25), HoverBaseBright))
		.SetDropIndicator_Below(BorderBrush("/Elements/Table/DropZoneIndicator_Below.png", FMargin(10.0f / 16.0f, 0, 0, 10.0f / 16.0f), HoverBaseBright));
	CoreStyle->Set("TableView.Row", FTableRowStyle(DefaultTableRowStyle));
	EditorStyle->Set("TableView.Row", FTableRowStyle(DefaultTableRowStyle));

	// SmallRounded

	BrushDatabase->GetDynamicMaterial("Button_Start")->SetScalarParameterValue("Part", -1);
	BrushDatabase->GetDynamicMaterial("Button_Start_Hovered")->SetScalarParameterValue("Part", -1);
	BrushDatabase->GetDynamicMaterial("Button_Start_Pressed")->SetScalarParameterValue("Part", -1);
	BrushDatabase->GetDynamicMaterial("Button_Start_Checked")->SetScalarParameterValue("Part", -1);
	BrushDatabase->GetDynamicMaterial("Button_Start_Hovered_Checked")->SetScalarParameterValue("Part", -1);

	BrushDatabase->GetDynamicMaterial("Button_Middle")->SetScalarParameterValue("Part", 0);
	BrushDatabase->GetDynamicMaterial("Button_Middle_Hovered")->SetScalarParameterValue("Part", 0);
	BrushDatabase->GetDynamicMaterial("Button_Middle_Pressed")->SetScalarParameterValue("Part", 0);
	BrushDatabase->GetDynamicMaterial("Button_Middle_Checked")->SetScalarParameterValue("Part", 0);
	BrushDatabase->GetDynamicMaterial("Button_Middle_Hovered_Checked")->SetScalarParameterValue("Part", 0);

	BrushDatabase->GetDynamicMaterial("Button_End")->SetScalarParameterValue("Part", 1);
	BrushDatabase->GetDynamicMaterial("Button_End_Hovered")->SetScalarParameterValue("Part", 1);
	BrushDatabase->GetDynamicMaterial("Button_End_Pressed")->SetScalarParameterValue("Part", 1);
	BrushDatabase->GetDynamicMaterial("Button_End_Checked")->SetScalarParameterValue("Part", 1);
	BrushDatabase->GetDynamicMaterial("Button_End_Hovered_Checked")->SetScalarParameterValue("Part", 1);

	EditorStyle->Set("EditorViewportToolBar.MenuButton", FButtonStyle(Button));
	EditorStyle->Set("EditorViewportToolBar.MenuButtonWarning", FButtonStyle(Button));
	EditorStyle->Set("ViewportPinnedCommandList.Button", FButtonStyle(Button));
	EditorStyle->Set("ViewportMenu.Button", FButtonStyle(Button));
	EditorStyle->Set(
		"ViewportMenu.Button.Start",
		FButtonStyle(Button)
		.SetNormal(*SlateBrush("Button_Start"))
		.SetHovered(*SlateBrush("Button_Start_Hovered"))
		.SetPressed(*SlateBrush("Button_Start_Pressed")));
	EditorStyle->Set(
		"ViewportMenu.Button.Middle",
		FButtonStyle(Button)
		.SetNormal(*SlateBrush("Button_Middle"))
		.SetHovered(*SlateBrush("Button_Middle_Hovered"))
		.SetPressed(*SlateBrush("Button_Middle_Pressed")));
	EditorStyle->Set(
		"ViewportMenu.Button.End",
		FButtonStyle(Button)
		.SetNormal(*SlateBrush("Button_End"))
		.SetHovered(*SlateBrush("Button_End_Hovered"))
		.SetPressed(*SlateBrush("Button_End_Pressed")));

	const FCheckBoxStyle ToggleStart =
		FCheckBoxStyle()
		.SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
		.SetUncheckedImage(*SlateBrush("Button_Start"))
		.SetUncheckedHoveredImage(*SlateBrush("Button_Start_Hovered"))
		.SetUncheckedPressedImage(*SlateBrush("Button_Start_Pressed"))
		.SetCheckedImage(*SlateBrush("Button_Start_Checked"))
		.SetCheckedHoveredImage(*SlateBrush("Button_Start_Hovered_Checked"))
		.SetCheckedPressedImage(*SlateBrush("Button_Start_Pressed"))
		.SetPadding(Margin);

	const FCheckBoxStyle ToggleMiddle =
		FCheckBoxStyle()
		.SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
		.SetUncheckedImage(*SlateBrush("Button_Middle"))
		.SetUncheckedHoveredImage(*SlateBrush("Button_Middle_Hovered"))
		.SetUncheckedPressedImage(*SlateBrush("Button_Middle_Pressed"))
		.SetCheckedImage(*SlateBrush("Button_Middle_Checked"))
		.SetCheckedHoveredImage(*SlateBrush("Button_Middle_Hovered_Checked"))
		.SetCheckedPressedImage(*SlateBrush("Button_Middle_Pressed"))
		.SetPadding(Margin);

	const FCheckBoxStyle ToggleEnd =
		FCheckBoxStyle()
		.SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
		.SetUncheckedImage(*SlateBrush("Button_End"))
		.SetUncheckedHoveredImage(*SlateBrush("Button_End_Hovered"))
		.SetUncheckedPressedImage(*SlateBrush("Button_End_Pressed"))
		.SetCheckedImage(*SlateBrush("Button_End_Checked"))
		.SetCheckedHoveredImage(*SlateBrush("Button_End_Hovered_Checked"))
		.SetCheckedPressedImage(*SlateBrush("Button_End_Pressed"))
		.SetPadding(Margin);


	EditorStyle->Set("ViewportMenu.ToggleButton.Start", ToggleStart);
	EditorStyle->Set("ViewportMenu.ToggleButton.Middle", ToggleMiddle);
	EditorStyle->Set("ViewportMenu.ToggleButton.End", ToggleEnd);

	EditorStyle->Set("Property.ToggleButton.Start", ToggleStart);
	EditorStyle->Set("Property.ToggleButton.Middle", ToggleMiddle);
	EditorStyle->Set("Property.ToggleButton.End", ToggleEnd);

	CoreStyle->Set(
		"ToolBar.Button",
		FButtonStyle(Button)
		.SetNormal(FSlateNoResource())
		.SetPressed(*SlateBrush("HoverDark"))
		.SetHovered(*SlateBrush("HoverDark"))
		.SetNormalPadding(SmallMargin)
		.SetPressedPadding(SmallMargin)
	);


	CoreStyle->Set("ToolBar.Button.Pressed", SlateBrush("HoverDark"));
	CoreStyle->Set("ToolBar.Button.Hovered", SlateBrush("HoverDark"));

	CoreStyle->Set("ToolBar.Button.Checked", SlateBrush("HoverBase"));
	CoreStyle->Set("ToolBar.Button.Checked_Hovered", SlateBrush("HoverDark"));
	CoreStyle->Set("ToolBar.Button.Checked_Pressed", SlateBrush("HoverDark"));

	EditorStyle->Set("PropertyWindow.CategoryBackground", SlateBrush("Button"));

	CoreStyle->Set(
		"Menu.Button",
		FButtonStyle(Button)
		.SetNormal(FSlateNoResource())
		.SetHovered(*SlateBrush("Primary"))
		.SetPressed(*SlateBrush("Primary"))
		.SetNormalPadding(FMargin(0, 1))
		.SetPressedPadding(FMargin(0, 2, 0, 0))
	);

	CoreStyle->Set("Menu.Button.Checked", SlateBrush("HoverDark"));
	CoreStyle->Set("Menu.Button.Checked_Hovered", SlateBrush("HoverDark"));
	CoreStyle->Set("Menu.Button.Checked_Pressed", SlateBrush("HoverDark"));
	CoreStyle->Set("Menu.Button.SubMenuOpen", SlateBrush("HoverDark"));

	const FButtonStyle ToggleButton =
		FButtonStyle(Button)
		.SetNormal(FSlateNoResource())
		.SetHovered(*SlateBrush("HoverBase"))
		.SetPressed(*SlateBrush("HoverBase"))
		.SetNormalPadding(FMargin(0))
		.SetPressedPadding(FMargin(0));

	const FButtonStyle RoundButton =
		FButtonStyle(ToggleButton)
		.SetNormal(*SlateBrush("GreyBase"))
		.SetHovered(*SlateBrush("HoverBase"))
		.SetPressed(*SlateBrush("HoverBase"));
	EditorStyle->Set("ToggleButton", ToggleButton);
	EditorStyle->Set("FlatButton", ToggleButton);

	//FSlateColorBrush(FLinearColor::White)

	EditorStyle->Set("RoundButton", RoundButton);
	EditorStyle->Set(
		"FlatButton",
		FButtonStyle(Button)
		.SetNormal(*SlateBrush("GreyBase"))
		.SetHovered(*SlateBrush("HoverBase"))
		.SetPressed(*SlateBrush("HoverBase"))
	);
	EditorStyle->Set("FlatButton.Dark", RoundButton);
	EditorStyle->Set("FlatButton.DarkGrey", RoundButton);
	EditorStyle->Set("FlatButton.Light", RoundButton);
	EditorStyle->Set("FlatButton.Default", RoundButton);


	struct ButtonColor
	{
		FName Name;
		FColor Normal;
		FColor Hovered;

		ButtonColor(const FName& InName, const FColor& Color) : Name(InName)
		{
			Normal = Color;
			Normal.A = Color.A * 0.75;
			Hovered = Color;
			Hovered.A = Color.A * 1.0;
		}
	};

	// Red #b20000
	// Blue #0060b2
	// Yellow #dfb300
	// Green #3aa111

	TArray<ButtonColor> FlatButtons;
	FlatButtons.Add(ButtonColor("FlatButton.Primary", ButtonPrimary));
	FlatButtons.Add(ButtonColor("FlatButton.Success", ButtonSuccess));
	FlatButtons.Add(ButtonColor("FlatButton.Info", ButtonInfo));
	FlatButtons.Add(ButtonColor("FlatButton.Warning", ButtonWarning));
	FlatButtons.Add(ButtonColor("FlatButton.Danger", ButtonDanger));

	for (const ButtonColor& Entry : FlatButtons)
	{
		EditorStyle->Set(
			Entry.Name,
			FButtonStyle(Button)
			.SetNormal(BoxBrush("/Elements/Button/FlatButton.png", Icon0, 0.25, Entry.Normal))
			.SetHovered(BoxBrush("/Elements/Button/FlatButton.png", Icon0, 0.25, Entry.Hovered))
			.SetPressed(BoxBrush("/Elements/Button/FlatButton.png", Icon0, 0.25, Entry.Hovered))
		);
	}
}
