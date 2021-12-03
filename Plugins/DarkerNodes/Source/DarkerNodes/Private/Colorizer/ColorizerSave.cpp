/* Copyright (C) 2021 Hugo ATTAL - All Rights Reserved
* This plugin is downloadable from the Unreal Engine Marketplace
*/

#include "Colorizer.h"

void FColorizer::SaveStyleForUMG()
{
	// Hack to save the slates for UMG

	CoreStyle->Set("Saved_Checkbox", CoreStyle->GetWidgetStyle<FCheckBoxStyle>("Checkbox"));
	CoreStyle->Set("Saved_SpinBox", CoreStyle->GetWidgetStyle<FSpinBoxStyle>("SpinBox"));
	CoreStyle->Set("Saved_Button", CoreStyle->GetWidgetStyle<FButtonStyle>("Button"));
}