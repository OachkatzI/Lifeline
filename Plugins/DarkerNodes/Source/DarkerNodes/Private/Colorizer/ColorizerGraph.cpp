/* Copyright (C) 2021 Hugo ATTAL - All Rights Reserved
* This plugin is downloadable from the Unreal Engine Marketplace
*/

#include "Colorizer.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Classes/EditorStyleSettings.h"

void FColorizer::ColorGraph()
{
	// Graph Nodes

	if (DarkerNodesSettings->ActivateBlueprintTheme)
	{
		BrushDatabase->GetDynamicMaterial("RegularNode_body")->SetScalarParameterValue("Size", 32);
		SlateBrush("RegularNode_body")->ImageSize = FVector2D(32, 32);
		SlateBrush("RegularNode_body")->Margin = FMargin(0.5f);
		SlateBrush("RegularNode_body")->DrawAs = ESlateBrushDrawType::Box;

		BrushDatabase->GetDynamicMaterial("VarNode_body")->SetScalarParameterValue("Size", 32);
		SlateBrush("VarNode_body")->ImageSize = FVector2D(32, 32);
		SlateBrush("VarNode_body")->Margin = FMargin(0.5f);
		SlateBrush("VarNode_body")->DrawAs = ESlateBrushDrawType::Box;

		BrushDatabase->GetDynamicMaterial("RegularNode_color_spill")->SetScalarParameterValue("Size", 32);
		SlateBrush("RegularNode_color_spill")->ImageSize = FVector2D(32, 32);
		SlateBrush("RegularNode_color_spill")->Margin = FMargin(0.5f);
		SlateBrush("RegularNode_color_spill")->DrawAs = ESlateBrushDrawType::Box;

		EditorStyle->Set("Graph.Node.Body", SlateBrush("RegularNode_body"));
		EditorStyle->Set("Graph.VarNode.Body", SlateBrush("VarNode_body"));

		BrushDatabase->GetDynamicMaterial("RegularNode_shadow_selected")->SetScalarParameterValue("Size", 64);
		BrushDatabase->GetDynamicMaterial("VarNode_shadow_selected")->SetScalarParameterValue("Size", 64);

		SlateBrush("RegularNode_shadow_selected")->ImageSize = FVector2D(64, 64);
		SlateBrush("RegularNode_shadow_selected")->Margin = FMargin(0.5f);
		SlateBrush("RegularNode_shadow_selected")->DrawAs = ESlateBrushDrawType::Box;

		SlateBrush("VarNode_shadow_selected")->ImageSize = FVector2D(64, 64);
		SlateBrush("VarNode_shadow_selected")->Margin = FMargin(0.5f);
		SlateBrush("VarNode_shadow_selected")->DrawAs = ESlateBrushDrawType::Box;

		EditorStyle->Set("Graph.PlayInEditor", SlateBrush("RegularNode_shadow_selected"));
		EditorStyle->Set("Graph.Node.ShadowSelected", SlateBrush("RegularNode_shadow_selected"));
		EditorStyle->Set("Graph.VarNode.ShadowSelected", SlateBrush("VarNode_shadow_selected"));

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
		ApplyBoxBrush(EditorStyle, "Graph.Node.TitleGloss", "/Graph/RegularNode_color_spill_424.png");
		EditorStyle->Set("Graph.Node.ColorSpill", new FSlateNoResource());
#else
		EditorStyle->Set("Graph.Node.ColorSpill", SlateBrush("RegularNode_color_spill"));
		EditorStyle->Set("Graph.Node.TitleGloss", new FSlateNoResource());
#endif

		EditorStyle->Set("Graph.Node.Shadow", new FSlateNoResource());
		EditorStyle->Set("Graph.Node.TitleHighlight", new FSlateNoResource());

		switch (DarkerNodesSettings->BlueprintVarNodeLine)
		{
		case EBlueprintVarNodeLine::Thin:
			ApplyImageBrush(EditorStyle, "Graph.VarNode.ColorSpill", "/Graph/VarNode_color_spill_thin.png", FVector2D(132, 28));
			break;
		case EBlueprintVarNodeLine::Thick:
			ApplyImageBrush(EditorStyle, "Graph.VarNode.ColorSpill", "/Graph/VarNode_color_spill_thick.png", FVector2D(132, 28));
			break;
		}

		EditorStyle->Set("Graph.VarNode.Gloss", new FSlateNoResource());
		EditorStyle->Set("Graph.VarNode.Shadow", new FSlateNoResource());

		//EditorStyle->Set("Graph.VarNode.Body", SlateBrush("Button_Hovered"));

		EditorStyle->Set("Graph.CollapsedNode.Body", SlateBrush("RegularNode_body"));
		ApplyBoxBrush(EditorStyle, "Graph.CollapsedNode.BodyColorSpill", "/Graph/CollapsedNode_Body_ColorSpill.png");

		// Pin Icons

		ApplyImageBrush(EditorStyle, "Graph.Pin.Connected_VarA", "/Elements/Pin/Pin_connected_VarA.png", FVector2D(15, 11));
		ApplyImageBrush(EditorStyle, "Graph.Pin.Disconnected_VarA", "/Elements/Pin/Pin_disconnected_VarA.png", FVector2D(15, 11));
		ApplyImageBrush(EditorStyle, "Graph.Pin.Connected", "/Elements/Pin/Pin_connected.png", FVector2D(11, 11));
		ApplyImageBrush(EditorStyle, "Graph.Pin.Disconnected", "/Elements/Pin/Pin_disconnected.png", FVector2D(11, 11));

		ApplyImageBrush(EditorStyle, "Graph.ExecPin.Connected", "/Elements/Pin/ExecPin_Connected.png", FVector2D(12, 16));
		ApplyImageBrush(EditorStyle, "Graph.ExecPin.Disconnected", "/Elements/Pin/ExecPin_Disconnected.png", FVector2D(12, 16));
		ApplyImageBrush(EditorStyle, "Graph.ExecPin.ConnectedHovered", "/Elements/Pin/ExecPin_Connected.png", FVector2D(12, 16), FColor(150, 150, 150));
		ApplyImageBrush(EditorStyle, "Graph.ExecPin.DisconnectedHovered", "/Elements/Pin/ExecPin_Disconnected.png", FVector2D(12, 16), FColor(150, 150, 150));
		ApplyImageBrush(EditorStyle, "Graph.ExecutionBubble", "/Elements/Pin/ExecutionBubble.png", FVector2D(16, 16));
	}

	// Grid

	if (DarkerNodesSettings->UseCustomPanels)
	{
		EditorStyle->Set("Graph.Panel.GridLineColor", BrushDatabase->GetColor("GridLine").Get());
		GetMutableDefault<UEditorStyleSettings>()->RegularColor = BrushDatabase->GetColor("GridLine").Get();

		EditorStyle->Set("Graph.Panel.GridRuleColor", BrushDatabase->GetColor("GridRule").Get());
		GetMutableDefault<UEditorStyleSettings>()->RuleColor = BrushDatabase->GetColor("GridRule").Get();

		EditorStyle->Set("Graph.Panel.GridCenterColor", BrushDatabase->GetColor("GridCenter").Get());
		GetMutableDefault<UEditorStyleSettings>()->CenterColor = BrushDatabase->GetColor("GridCenter").Get();
	}

	ApplyBoxBrush(EditorStyle, "Kismet.Comment.Background", "/Graph/Comment_Background.png");

	// Comments

	ApplyBoxBrush(EditorStyle, "Graph.Node.CommentBubble", "/Graph/CommentBubble.png");
	ApplyImageBrush(EditorStyle, "Graph.Node.CommentArrow", "/Graph/CommentBubbleArrow.png", Icon8);

	// Panel

	ApplyImageBrush(EditorStyle, "BlueprintEditor.Details.ArgUpButton", "/Icons/Graph/icon_FunctionArgUp.png", Icon16);
	ApplyImageBrush(EditorStyle, "BlueprintEditor.Details.ArgDownButton", "/Icons/Graph/icon_FunctionArgDown.png", Icon16);
}
