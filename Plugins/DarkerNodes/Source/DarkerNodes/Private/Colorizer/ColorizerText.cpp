/* Copyright (C) 2021 Hugo ATTAL - All Rights Reserved
* This plugin is downloadable from the Unreal Engine Marketplace
*/

#include "Colorizer.h"

void FColorizer::ColorText()
{
	if (!DarkerNodesSettings->UseCustomButton)
	{
		return;
	}

	CoreStyle->Set("NormalFont", FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(9)));
	CoreStyle->Set("SmallFont", FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(8)));

	EditorStyle->Set("NormalFont", FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(9)));
	EditorStyle->Set("BoldFont", FCoreStyle::GetDefaultFontStyle("Bold", GetFontSize(9)));

	// FLinearColor(0.19f, 0.33f, 0.72f);

	NormalText =
		FTextBlockStyle()
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(9)))
		.SetColorAndOpacity(*BrushDatabase->GetSlateColor("TextColor"))
		.SetShadowOffset(FVector2D::ZeroVector)
		.SetShadowColorAndOpacity(FLinearColor::FromSRGBColor(TextShadow))
		.SetHighlightColor(FLinearColor::FromSRGBColor(Primary));

	const FTextBlockStyle ShadowText =
		FTextBlockStyle(NormalText)
		.SetShadowOffset(FVector2D(1.0f, 1.0f));

	CoreStyle->Set("NormalText", NormalText);

	CoreStyle->Set(
		"SmallText",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(8))));

	EditorStyle->Set("NormalText", NormalText);

	EditorStyle->Set(
		"NormalText.Subdued",
		FTextBlockStyle(NormalText)
		.SetColorAndOpacity(FSlateColor::UseSubduedForeground()));

	EditorStyle->Set(
		"NormalText.Important",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Bold", GetFontSize(9)))
		.SetShadowOffset(FVector2D(1, 1)));

	EditorStyle->Set(
		"SmallText",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(8))));

	EditorStyle->Set(
		"SmallText.Subdued",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(8)))
		.SetColorAndOpacity(FSlateColor::UseSubduedForeground()));

	EditorStyle->Set(
		"TinyText",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(8))));

	EditorStyle->Set(
		"TinyText.Subdued",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(8)))
		.SetColorAndOpacity(FSlateColor::UseSubduedForeground()));

	EditorStyle->Set(
		"LargeText",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Bold", GetFontSize(11)))
		.SetShadowOffset(FVector2D(1, 1)));

	const FEditableTextBoxStyle NormalEditableTextBoxStyle =
		FEditableTextBoxStyle()
		.SetFont(NormalText.Font)
		.SetBackgroundImageNormal(*SlateBrush("Button"))
		.SetBackgroundImageHovered(*SlateBrush("Button_Hovered"))
		.SetBackgroundImageFocused(*SlateBrush("Button_Hovered"))
		.SetBackgroundImageReadOnly(*SlateBrush("Button_Disabled"))
		.SetForegroundColor(FLinearColor::FromSRGBColor(TextColor))
		.SetScrollBarStyle(CoreStyle->GetWidgetStyle<FScrollBarStyle>("Scrollbar"));

	CoreStyle->Set("NormalEditableTextBox", NormalEditableTextBoxStyle);
	EditorStyle->Set("NormalEditableTextBox", NormalEditableTextBoxStyle);

	CoreStyle->Set(
		"EmbossedText",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(24)))
		.SetColorAndOpacity(*BrushDatabase->GetSlateColor("TextColor"))
		.SetShadowOffset(FVector2D(1.0f, 1.0f))
	);

	CoreStyle->Set(
		"SearchBox",
		FSearchBoxStyle()
		.SetTextBoxStyle(NormalEditableTextBoxStyle)
		.SetUpArrowImage(ImageBrush("/Elements/Textbox/UpArrow.png", Icon8))
		.SetDownArrowImage(ImageBrush("/Elements/Textbox/DownArrow.png", Icon8))
		.SetGlassImage(ImageBrush("/Elements/Textbox/SearchGlass.png", Icon16))
		.SetClearImage(ImageBrush("/Elements/Textbox/X.png", Icon16))
	);

	EditorStyle->Set(
		"LevelViewportContextMenu.ActorType.Text",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(8))));

	EditorStyle->Set(
		"LevelViewportContextMenu.AssetLabel.Text",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(9))));

	EditorStyle->Set(
		"EditorModesToolbar.Label",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(7))));
	EditorStyle->Set(
		"EditorModesToolbar.EditableText",
		FEditableTextBoxStyle(NormalEditableTextBoxStyle)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(9))));
	EditorStyle->Set(
		"EditorModesToolbar.Keybinding",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(8))));

	EditorStyle->Set(
		"ToolBar.Label",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(9))));
	EditorStyle->Set(
		"ToolBar.EditableText",
		FEditableTextBoxStyle(NormalEditableTextBoxStyle)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(9))));
	EditorStyle->Set(
		"ToolBar.Keybinding",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(8))));

	EditorStyle->Set(
		"ToolBar.Heading",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(8))));

	EditorStyle->Set(
		"ViewportMenu.Label",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Bold", GetFontSize(9))));

	EditorStyle->Set(
		"SProjectBadge.Text",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(12))));

	EditorStyle->Set(
		"Editor.SearchBoxFont",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(12))));

	EditorStyle->Set(
		"ViewportPinnedCommandList.Label",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Bold", GetFontSize(9))));

	EditorStyle->Set(
		"ViewportPinnedCommandList.EditableText",
		FEditableTextBoxStyle(NormalEditableTextBoxStyle)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Bold", GetFontSize(9))));

	EditorStyle->Set(
		"ViewportPinnedCommandList.Keybinding",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(8))));

	EditorStyle->Set(
		"ViewportPinnedCommandList.Heading",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(8))));

	EditorStyle->Set(
		"PlacementBrowser.Tab.Text",
		FTextBlockStyle(ShadowText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Bold", GetFontSize(10))));

	EditorStyle->Set(
		"PlacementBrowser.Asset.Name",
		FTextBlockStyle(ShadowText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(10))));

	EditorStyle->Set(
		"PlacementBrowser.Asset.Type",
		FTextBlockStyle(ShadowText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(8))));

	EditorStyle->Set(
		"FlatButton.DefaultTextStyle",
		FTextBlockStyle(ShadowText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Bold", GetFontSize(10)))
		.SetHighlightColor(FLinearColor::FromSRGBColor(Primary)));

	EditorStyle->Set(
		"Profiler.EventGraph.DarkText",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(8)))
	);

	EditorStyle->Set(
		"WorldBrowser.LabelFont",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", GetFontSize(9)))
	);

	EditorStyle->Set(
		"WorldBrowser.LabelFontBold",
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Bold", GetFontSize(10)))
	);

	const FString FontAwesomePath = FPaths::EngineContentDir() / TEXT("Editor/Slate/Fonts/FontAwesome.ttf");

	EditorStyle->Set(
		"FindResults.FindInBlueprints",
		FTextBlockStyle(NormalText)
		.SetFont(FSlateFontInfo(FontAwesomePath, GetFontSize(10))));

	// ----

	const FInlineEditableTextBlockStyle InlineEditableTextBlockStyle =
		FInlineEditableTextBlockStyle()
		.SetTextStyle(NormalText)
		.SetEditableTextBoxStyle(NormalEditableTextBoxStyle);

	EditorStyle->Set("InlineEditableTextBlockStyle", InlineEditableTextBlockStyle);
	CoreStyle->Set("InlineEditableTextBlockStyle", InlineEditableTextBlockStyle);
	EditorStyle->Set("Graph.Node.InlineEditablePinName", InlineEditableTextBlockStyle);

	const FTextBlockStyle GraphNodeTitle =
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Bold", GetFontSize(10)));

	EditorStyle->Set("Graph.Node.NodeTitle", GraphNodeTitle);
	EditorStyle->Set("Graph.Node.NodeTitleEditableText", NormalEditableTextBoxStyle);
	EditorStyle->Set("Graph.StateNode.NodeTitleEditableText", NormalEditableTextBoxStyle);

	EditorStyle->Set(
		"Graph.Node.NodeTitleInlineEditableText",
		FInlineEditableTextBlockStyle()
		.SetTextStyle(GraphNodeTitle)
		.SetEditableTextBoxStyle(NormalEditableTextBoxStyle)
	);

	EditorStyle->Set(
		"Graph.StateNode.NodeTitleInlineEditableText",
		FInlineEditableTextBlockStyle()
		.SetTextStyle(GraphNodeTitle)
		.SetEditableTextBoxStyle(NormalEditableTextBoxStyle)
	);

	// ----

	const FTextBlockStyle GraphCommentBlockTitle =
		FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::GetDefaultFontStyle("Bold", GetFontSize(18)))
		.SetColorAndOpacity(FSlateColor(FColor::White))
		.SetShadowOffset(FVector2D(1.5f, 1.5f))
		.SetShadowColorAndOpacity(FLinearColor::FromSRGBColor(TextShadow));
	EditorStyle->Set("Graph.CommentBlock.Title", GraphCommentBlockTitle);

	const FEditableTextBoxStyle GraphCommentBlockTitleEditableText =
		FEditableTextBoxStyle(NormalEditableTextBoxStyle)
		.SetFont(GraphCommentBlockTitle.Font);
	EditorStyle->Set("Graph.CommentBlock.TitleEditableText", GraphCommentBlockTitleEditableText);

	EditorStyle->Set(
		"Graph.CommentBlock.TitleInlineEditableText",
		FInlineEditableTextBlockStyle()
		.SetTextStyle(GraphCommentBlockTitle)
		.SetEditableTextBoxStyle(GraphCommentBlockTitleEditableText)
	);
}

int32 FColorizer::GetFontSize(int32 BaseSize) const
{
	switch (DarkerNodesSettings->FontSize)
	{
	case EFontSize::Small:
		return BaseSize * 0.8f;
	case EFontSize::Normal:
		return BaseSize;
	case EFontSize::Big:
		return BaseSize * 1.2f;
	case EFontSize::Bigger:
		return BaseSize * 1.5f;
	default:
		return BaseSize;
	}
}
