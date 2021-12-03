/* Copyright (C) 2021 Hugo ATTAL - All Rights Reserved
* This plugin is downloadable from the Unreal Engine Marketplace
*/

#include "Colorizer.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Classes/EditorStyleSettings.h"

void FColorizer::ApplyParameters() const
{
	ApplyFonts();

	BrushDatabase->GetDynamicMaterial("GreyBase")->SetVectorParameterValue("Color", FLinearColor::FromSRGBColor(GreyBase));
	BrushDatabase->GetDynamicMaterial("GreyLight")->SetVectorParameterValue("Color", FLinearColor::FromSRGBColor(GreyLight));
	BrushDatabase->GetDynamicMaterial("GreyDark")->SetVectorParameterValue("Color", FLinearColor::FromSRGBColor(GreyDark));
	BrushDatabase->GetDynamicMaterial("Debug")->SetVectorParameterValue("Color", FLinearColor::FromSRGBColor(DebugRed));

	BrushDatabase->GetDynamicMaterial("HoverDark")->SetVectorParameterValue("Color", FLinearColor::FromSRGBColor(HoverDark));
	BrushDatabase->GetDynamicMaterial("HoverBase")->SetVectorParameterValue("Color", FLinearColor::FromSRGBColor(HoverBase));
	BrushDatabase->GetDynamicMaterial("HoverBaseBright")->SetVectorParameterValue("Color", FLinearColor::FromSRGBColor(HoverBaseBright));

	BrushDatabase->GetDynamicMaterial("Primary")->SetVectorParameterValue("Color", FLinearColor::FromSRGBColor(Primary));
	BrushDatabase->SetSlateColor("TextColor", FLinearColor::FromSRGBColor(TextColor));
	BrushDatabase->GetDynamicMaterial("ScrollbarColor")->SetVectorParameterValue("Color", FLinearColor::FromSRGBColor(ScrollbarColor));
	BrushDatabase->GetDynamicMaterial("MainWindowColor")->SetVectorParameterValue("Color", FLinearColor::FromSRGBColor(MainWindowColor));
	BrushDatabase->GetDynamicMaterial("ChildWindowColor")->SetVectorParameterValue("Color", FLinearColor::FromSRGBColor(ChildWindowColor));

	BrushDatabase->SetSlateColor("GridLine", FLinearColor::FromSRGBColor(GridLineColor));
	BrushDatabase->SetSlateColor("GridRule", FLinearColor::FromSRGBColor(GridRuleColor));
	BrushDatabase->SetSlateColor("GridCenter", FLinearColor::FromSRGBColor(GridCenterColor));

	if (DarkerNodesSettings->UseCustomPanels)
	{
		GetMutableDefault<UEditorStyleSettings>()->RegularColor = BrushDatabase->GetColor("GridLine").Get();
		GetMutableDefault<UEditorStyleSettings>()->RuleColor = BrushDatabase->GetColor("GridRule").Get();
		GetMutableDefault<UEditorStyleSettings>()->CenterColor = BrushDatabase->GetColor("GridCenter").Get();
		GetMutableDefault<UEditorStyleSettings>()->SaveConfig();
	}

	BrushDatabase->GetDynamicMaterial("Button")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(GreyDark));
	BrushDatabase->GetDynamicMaterial("Button")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(ButtonBorderColor));
	BrushDatabase->GetDynamicMaterial("Button")->SetScalarParameterValue("Radius", DarkerNodesSettings->ButtonRadius);
	BrushDatabase->GetDynamicMaterial("Button_Hovered")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(GreyDark));
	BrushDatabase->GetDynamicMaterial("Button_Hovered")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(Primary));
	BrushDatabase->GetDynamicMaterial("Button_Hovered")->SetScalarParameterValue("Radius", DarkerNodesSettings->ButtonRadius);
	BrushDatabase->GetDynamicMaterial("Button_Pressed")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(HoverDark));
	BrushDatabase->GetDynamicMaterial("Button_Pressed")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(Primary));
	BrushDatabase->GetDynamicMaterial("Button_Pressed")->SetScalarParameterValue("Radius", DarkerNodesSettings->ButtonRadius);
	BrushDatabase->GetDynamicMaterial("Button_Disabled")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(GreyDark));
	BrushDatabase->GetDynamicMaterial("Button_Disabled")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(GreyDark));
	BrushDatabase->GetDynamicMaterial("Button_Disabled")->SetScalarParameterValue("Radius", DarkerNodesSettings->ButtonRadius);
	BrushDatabase->GetDynamicMaterial("Button_Disabled")->SetScalarParameterValue("Alpha", 0.5);

	BrushDatabase->GetDynamicMaterial("Button_Start")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(GreyDark));
	BrushDatabase->GetDynamicMaterial("Button_Start")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(ButtonBorderColor));
	BrushDatabase->GetDynamicMaterial("Button_Start")->SetScalarParameterValue("Radius", DarkerNodesSettings->ButtonRadius);
	BrushDatabase->GetDynamicMaterial("Button_Start_Hovered")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(GreyDark));
	BrushDatabase->GetDynamicMaterial("Button_Start_Hovered")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(Primary));
	BrushDatabase->GetDynamicMaterial("Button_Start_Hovered")->SetScalarParameterValue("Radius", DarkerNodesSettings->ButtonRadius);
	BrushDatabase->GetDynamicMaterial("Button_Start_Pressed")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(HoverDark));
	BrushDatabase->GetDynamicMaterial("Button_Start_Pressed")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(Primary));
	BrushDatabase->GetDynamicMaterial("Button_Start_Pressed")->SetScalarParameterValue("Radius", DarkerNodesSettings->ButtonRadius);
	BrushDatabase->GetDynamicMaterial("Button_Start_Checked")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(HoverBaseBright));
	BrushDatabase->GetDynamicMaterial("Button_Start_Checked")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(HoverBaseBright));
	BrushDatabase->GetDynamicMaterial("Button_Start_Checked")->SetScalarParameterValue("Radius", DarkerNodesSettings->ButtonRadius);
	BrushDatabase->GetDynamicMaterial("Button_Start_Hovered_Checked")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(HoverBaseBright));
	BrushDatabase->GetDynamicMaterial("Button_Start_Hovered_Checked")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(Primary));
	BrushDatabase->GetDynamicMaterial("Button_Start_Hovered_Checked")->SetScalarParameterValue("Radius", DarkerNodesSettings->ButtonRadius);

	BrushDatabase->GetDynamicMaterial("Button_Middle")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(GreyDark));
	BrushDatabase->GetDynamicMaterial("Button_Middle")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(ButtonBorderColor));
	BrushDatabase->GetDynamicMaterial("Button_Middle_Hovered")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(GreyDark));
	BrushDatabase->GetDynamicMaterial("Button_Middle_Hovered")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(Primary));
	BrushDatabase->GetDynamicMaterial("Button_Middle_Pressed")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(HoverDark));
	BrushDatabase->GetDynamicMaterial("Button_Middle_Pressed")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(Primary));
	BrushDatabase->GetDynamicMaterial("Button_Middle_Checked")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(HoverBaseBright));
	BrushDatabase->GetDynamicMaterial("Button_Middle_Checked")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(HoverBaseBright));
	BrushDatabase->GetDynamicMaterial("Button_Middle_Hovered_Checked")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(HoverBaseBright));
	BrushDatabase->GetDynamicMaterial("Button_Middle_Hovered_Checked")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(Primary));

	BrushDatabase->GetDynamicMaterial("Button_End")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(GreyDark));
	BrushDatabase->GetDynamicMaterial("Button_End")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(ButtonBorderColor));
	BrushDatabase->GetDynamicMaterial("Button_End")->SetScalarParameterValue("Radius", DarkerNodesSettings->ButtonRadius);
	BrushDatabase->GetDynamicMaterial("Button_End_Hovered")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(GreyDark));
	BrushDatabase->GetDynamicMaterial("Button_End_Hovered")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(Primary));
	BrushDatabase->GetDynamicMaterial("Button_End_Hovered")->SetScalarParameterValue("Radius", DarkerNodesSettings->ButtonRadius);
	BrushDatabase->GetDynamicMaterial("Button_End_Pressed")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(HoverDark));
	BrushDatabase->GetDynamicMaterial("Button_End_Pressed")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(Primary));
	BrushDatabase->GetDynamicMaterial("Button_End_Pressed")->SetScalarParameterValue("Radius", DarkerNodesSettings->ButtonRadius);
	BrushDatabase->GetDynamicMaterial("Button_End_Checked")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(HoverBaseBright));
	BrushDatabase->GetDynamicMaterial("Button_End_Checked")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(HoverBaseBright));
	BrushDatabase->GetDynamicMaterial("Button_End_Checked")->SetScalarParameterValue("Radius", DarkerNodesSettings->ButtonRadius);
	BrushDatabase->GetDynamicMaterial("Button_End_Hovered_Checked")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(HoverBaseBright));
	BrushDatabase->GetDynamicMaterial("Button_End_Hovered_Checked")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(Primary));
	BrushDatabase->GetDynamicMaterial("Button_End_Hovered_Checked")->SetScalarParameterValue("Radius", DarkerNodesSettings->ButtonRadius);

	BrushDatabase->GetDynamicMaterial("RegularNode_body")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(RegularNodeBackground));
	BrushDatabase->GetDynamicMaterial("RegularNode_body")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(RegularNodeBorder));
	BrushDatabase->GetDynamicMaterial("RegularNode_body")->SetScalarParameterValue("Radius", DarkerNodesSettings->BlueprintRegularNodeRadius);

	BrushDatabase->GetDynamicMaterial("RegularNode_shadow_selected")->SetScalarParameterValue("Radius", DarkerNodesSettings->BlueprintRegularNodeRadius + 2);
	BrushDatabase->GetDynamicMaterial("RegularNode_shadow_selected")->SetVectorParameterValue("Border", Primary);
	BrushDatabase->GetDynamicMaterial("RegularNode_shadow_selected")->SetVectorParameterValue("Background", Primary.WithAlpha(0.0f));

	BrushDatabase->GetDynamicMaterial("RegularNode_color_spill")->SetScalarParameterValue("Radius", DarkerNodesSettings->BlueprintRegularNodeRadius);
	BrushDatabase->GetDynamicMaterial("RegularNode_color_spill")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(MixColor(FColor::Black, FColor::White, DarkerNodesSettings->BlueprintNodeHeaderSaturation)));
	BrushDatabase->GetDynamicMaterial("RegularNode_color_spill")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(MixColor(FColor::Black, FColor::White, DarkerNodesSettings->BlueprintNodeHeaderSaturation)));
	BrushDatabase->GetDynamicMaterial("RegularNode_color_spill")->SetScalarParameterValue("Alpha", DarkerNodesSettings->BlueprintNodeHeaderOpacity);

	BrushDatabase->GetDynamicMaterial("PanelDark")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(GreyDark));
	BrushDatabase->GetDynamicMaterial("PanelDark")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(GreyBase));

	BrushDatabase->GetDynamicMaterial("VarNode_body")->SetVectorParameterValue("Background", FLinearColor::FromSRGBColor(VarNodeBackground));
	BrushDatabase->GetDynamicMaterial("VarNode_body")->SetVectorParameterValue("Border", FLinearColor::FromSRGBColor(VarNodeBorder));
	BrushDatabase->GetDynamicMaterial("VarNode_body")->SetScalarParameterValue("Radius", DarkerNodesSettings->BlueprintVarNodeRadius);

	BrushDatabase->GetDynamicMaterial("VarNode_shadow_selected")->SetScalarParameterValue("Radius", DarkerNodesSettings->BlueprintVarNodeRadius + 2);
	BrushDatabase->GetDynamicMaterial("VarNode_shadow_selected")->SetVectorParameterValue("Border", Primary);
	BrushDatabase->GetDynamicMaterial("VarNode_shadow_selected")->SetVectorParameterValue("Background", Primary.WithAlpha(0.0f));

	if (DarkerNodesSettings->ExtendNodes)
	{
		BrushDatabase->GetDynamicMaterial("RegularNode_body")->SetVectorParameterValue("Padding", FLinearColor(0, 0, 4, 0));
		BrushDatabase->GetDynamicMaterial("VarNode_body")->SetVectorParameterValue("Padding", FLinearColor(0, 0, 4, 0));
		BrushDatabase->GetDynamicMaterial("RegularNode_shadow_selected")->SetVectorParameterValue("Padding", FLinearColor(10, 10, 14, 10));
		BrushDatabase->GetDynamicMaterial("VarNode_shadow_selected")->SetVectorParameterValue("Padding", FLinearColor(10, 10, 14, 10));
		BrushDatabase->GetDynamicMaterial("RegularNode_color_spill")->SetVectorParameterValue("Padding", FLinearColor(1, 1, 0, 0));
	}
	else
	{
		BrushDatabase->GetDynamicMaterial("RegularNode_body")->SetVectorParameterValue("Padding", FLinearColor(4, 2, 4, 2));
		BrushDatabase->GetDynamicMaterial("VarNode_body")->SetVectorParameterValue("Padding", FLinearColor(4, 2, 4, 2));
		BrushDatabase->GetDynamicMaterial("RegularNode_shadow_selected")->SetVectorParameterValue("Padding", FLinearColor(14, 12, 14, 12));
		BrushDatabase->GetDynamicMaterial("VarNode_shadow_selected")->SetVectorParameterValue("Padding", FLinearColor(14, 12, 14, 12));
		BrushDatabase->GetDynamicMaterial("RegularNode_color_spill")->SetVectorParameterValue("Padding", FLinearColor(5, 3, 0, 0));
	}
}
