/* Copyright (C) 2021 Hugo ATTAL - All Rights Reserved
* This plugin is downloadable from the Unreal Engine Marketplace
*/

#include "Colorizer.h"

void FColorizer::ColorCheckbox()
{
	if (!DarkerNodesSettings->UseCustomButton)
	{
		return;
	}
	
	// Border Opacity 15% / 75%
	// Front Opacity 75%
	
	const FCheckBoxStyle BasicCheckBoxStyle =
		FCheckBoxStyle()
		.SetCheckBoxType(ESlateCheckBoxType::CheckBox)

		.SetUncheckedImage(ImageBrush("/Elements/Checkbox/CheckBox.png"))
		.SetUncheckedHoveredImage(ImageBrush("/Elements/Checkbox/CheckBox_Hovered.png"))
		.SetUncheckedPressedImage(ImageBrush("/Elements/Checkbox/CheckBox_Hovered.png"))

		.SetCheckedImage(ImageBrush("/Elements/Checkbox/CheckBox_Checked.png"))
		.SetCheckedHoveredImage(ImageBrush("/Elements/Checkbox/CheckBox_Checked_Hovered.png"))
		.SetCheckedPressedImage(ImageBrush("/Elements/Checkbox/CheckBox_Checked_Hovered.png"))

		.SetUndeterminedImage(ImageBrush("/Elements/Checkbox/CheckBox_Undetermined.png"))
		.SetUndeterminedHoveredImage(ImageBrush("/Elements/Checkbox/CheckBox_Undetermined_Hovered.png"))
		.SetUndeterminedPressedImage(ImageBrush("/Elements/Checkbox/CheckBox_Undetermined_Hovered.png"));
	
	CoreStyle->Set("Checkbox", BasicCheckBoxStyle);
	
	EditorStyle->Set("Graph.Checkbox", BasicCheckBoxStyle);
	const FCheckBoxStyle BasicToggleButtonCheckBoxStyle =
		FCheckBoxStyle(BasicCheckBoxStyle)
		.SetCheckBoxType(ESlateCheckBoxType::ToggleButton);

	CoreStyle->Set("Menu.CheckBox", BasicCheckBoxStyle);
	CoreStyle->Set("Menu.Check", BasicCheckBoxStyle);
	CoreStyle->Set("Menu.ToggleButton", BasicToggleButtonCheckBoxStyle);


	const FCheckBoxStyle BasicRadioButtonStyle =
		FCheckBoxStyle()
		.SetUncheckedImage(ImageBrush("/Elements/Checkbox/RadioButton.png"))
		.SetUncheckedHoveredImage(ImageBrush("/Elements/Checkbox/RadioButton_Hovered.png"))
		.SetUncheckedPressedImage(ImageBrush("/Elements/Checkbox/RadioButton_Hovered.png"))

		.SetCheckedImage(ImageBrush("/Elements/Checkbox/RadioButton_Checked.png"))
		.SetCheckedHoveredImage(ImageBrush("/Elements/Checkbox/RadioButton_Checked_Hovered.png"))
		.SetCheckedPressedImage(ImageBrush("/Elements/Checkbox/RadioButton_Checked_Hovered.png"))

		.SetUndeterminedImage(ImageBrush("/Elements/Checkbox/RadioButton.png"))
		.SetUndeterminedHoveredImage(ImageBrush("/Elements/Checkbox/RadioButton_Hovered.png"))
		.SetUndeterminedPressedImage(ImageBrush("/Elements/Checkbox/RadioButton_Hovered.png"));

	CoreStyle->Set("RadioButton", BasicRadioButtonStyle);
	CoreStyle->Set("Menu.RadioButton", BasicRadioButtonStyle);
	CoreStyle->Set("ToolBar.RadioButton", BasicRadioButtonStyle);

	//EditorStyle->Set("RadioButton", BasicRadioButtonStyle);
	//EditorStyle->Set("ToolBar.RadioButton", BasicRadioButtonStyle);
	//EditorStyle->Set("NotificationBar.RadioButton", BasicRadioButtonStyle);
	//EditorStyle->Set("Menu.RadioButton", BasicRadioButtonStyle);
	//EditorStyle->Set("EditorModesToolbar.RadioButton", BasicRadioButtonStyle);
	//EditorStyle->Set("PinnedCommandList.RadioButton", BasicRadioButtonStyle);
	//EditorStyle->Set("ViewportPinnedCommandList.RadioButton", BasicRadioButtonStyle);
}
