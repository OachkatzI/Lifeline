/* Copyright (C) 2021 Hugo ATTAL - All Rights Reserved
* This plugin is downloadable from the Unreal Engine Marketplace
*/

#include "Colorizer.h"
#include "Classes/EditorStyleSettings.h"
#include "EditorStyle/Private/SlateEditorStyle.cpp"

void FColorizer::ColorPanel()
{	
	if (!DarkerNodesSettings->UseCustomPanels)
	{
		return;
	}

	const FButtonStyle CloseButton =
		FButtonStyle()
		.SetNormal(ImageBrush("/Elements/Panel/CloseApp_Normal.png", Icon16))
		.SetPressed(ImageBrush("/Elements/Panel/CloseApp_Pressed.png", Icon16))
		.SetHovered(ImageBrush("/Elements/Panel/CloseApp_Hovered.png", Icon16));

	//ApplyColorBorderBrush(CoreStyle, "Border", Green);
	CoreStyle->Set(
		"Docking.Tab",
		FDockTabStyle()
		.SetCloseButtonStyle(CloseButton)
		.SetNormalBrush(*SlateBrush("GreyDark"))
		.SetHoveredBrush(*SlateBrush("HoverDark"))
		.SetForegroundBrush(*SlateBrush("GreyBase"))
		.SetActiveBrush(*SlateBrush("GreyBase"))
		.SetColorOverlayTabBrush(*SlateBrush("HoverDark"))
		.SetColorOverlayIconBrush(*SlateBrush("HoverDark"))
		.SetContentAreaBrush(*SlateBrush("GreyDark"))
		.SetTabWellBrush(FSlateNoResource())
		.SetTabPadding(FMargin(5, 2, 5, 2))
		.SetOverlapWidth(-1.0f)
		.SetFlashColor(FSlateColor(FLinearColor::FromSRGBColor(HoverDark)))
	);

	CoreStyle->Set(
		"Docking.MajorTab",
		FDockTabStyle()
		.SetCloseButtonStyle(CloseButton)
		.SetNormalBrush(*SlateBrush("GreyBase"))
		.SetActiveBrush(*SlateBrush("HoverBase"))
		.SetColorOverlayTabBrush(*SlateBrush("GreyDark"))
		.SetColorOverlayIconBrush(*SlateBrush("GreyDark"))
		.SetForegroundBrush(*SlateBrush("GreyDark"))
		.SetHoveredBrush(*SlateBrush("HoverBase"))
		.SetContentAreaBrush(*SlateBrush("GreyDark"))
		.SetTabWellBrush(FSlateNoResource())
		.SetTabPadding(FMargin(8, 4, 8, 4))
		.SetOverlapWidth(0.0f)
		.SetFlashColor(FSlateColor(FLinearColor::FromSRGBColor(HoverBase)))
	);

	//EditorStyle->Set("ToolPalette.DockingWell", new FSlateColorBrush(FLinearColor(Blue)));

	CoreStyle->Set("ToolBar.Separator", SlateBrush("GreyDark"));

	EditorStyle->Set("Graph.Panel.SolidBackground", SlateBrush("GreyDark"));

	//ApplyImageBrush(EditorStyle, "Graph.Panel.SolidBackground", "/Graph/GraphPanel_SolidBackground.png", Icon16);

	//  new FSlateColorBrush(FLinearColor::FromSRGBColor(DebugRed))

	EditorStyle->Set("Window.Background", SlateBrush("GreyDark"));
	CoreStyle->Set("ToolBar.Background", SlateBrush("GreyDark"));
	EditorStyle->Set("ToolBar.Background", SlateBrush("GreyDark"));
	CoreStyle->Set("Menu.Background", SlateBrush("GreyDark"));
	EditorStyle->Set("Menu.Background", SlateBrush("GreyDark"));

	ApplyColorBoxBrush(EditorStyle, "Graph.InstructionBackground", FColor(40, 40, 40, 128));


	EditorStyle->Set("ContentBrowser.TileViewTooltip.ContentBorder", SlateBrush("GreyBase"));
	EditorStyle->Set("ProjectBrowser.TileViewTooltip.ContentBorder", SlateBrush("GreyBase"));

	CoreStyle->Set("ToolPanel.GroupBorder", SlateBrush("GreyBase"));
	EditorStyle->Set("ToolPanel.GroupBorder", SlateBrush("GreyBase"));

	EditorStyle->Set("PropertyTable.RowHeader.Background", SlateBrush("GreyDark"));
	EditorStyle->Set("PropertyTable.RowHeader.BackgroundActive", SlateBrush("HoverDark"));

	EditorStyle->Set(
		"PropertyTable.TableRow",
		FTableRowStyle()
		.SetEvenRowBackgroundBrush(FSlateColorBrush(FLinearColor::FromSRGBColor(MixColor(GreyBase, FColor::Black, 0.2))))
		.SetEvenRowBackgroundHoveredBrush(ColorImageBrush(HoverBase, Icon8))
		.SetOddRowBackgroundBrush(FSlateColorBrush(FLinearColor::FromSRGBColor(GreyBase)))
		.SetOddRowBackgroundHoveredBrush(ColorImageBrush(HoverBase, Icon8))
		.SetSelectorFocusedBrush(ColorBorderBrush(Primary))
		.SetActiveBrush(ColorImageBrush(HoverBase, Icon8))
		.SetActiveHoveredBrush(ColorImageBrush(HoverBase, Icon8))
		.SetInactiveBrush(ColorImageBrush(HoverBase, Icon8))
		.SetInactiveHoveredBrush(ColorImageBrush(HoverBase, Icon8))
		.SetTextColor(*BrushDatabase->GetSlateColor("TextColor"))
		.SetSelectedTextColor(*BrushDatabase->GetSlateColor("TextColor"))
	);

	EditorStyle->Set("SCSEditor.TreePanel", SlateBrush("GreyBase"));
	EditorStyle->Set("ContentBrowser.TopBar.GroupBorder", SlateBrush("GreyBase"));
	EditorStyle->Set("ToolPanel.DarkGroupBorder", SlateBrush("GreyBase"));
	EditorStyle->Set("ToolPanel.LightGroupBorder", SlateBrush("GreyBase"));

	EditorStyle->Set("DetailsView.CollapsedCategory", SlateBrush("GreyBase"));
	EditorStyle->Set("DetailsView.CollapsedCategory_Hovered", SlateBrush("HoverDark"));
	EditorStyle->Set("DetailsView.CategoryTop", SlateBrush("GreyBase"));
	EditorStyle->Set("DetailsView.CategoryTop_Hovered", SlateBrush("HoverDark"));
	EditorStyle->Set("DetailsView.CategoryMiddle", SlateBrush("GreyBase"));
	EditorStyle->Set("DetailsView.CategoryMiddle_Hovered", SlateBrush("HoverBase"));
	EditorStyle->Set("DetailsView.CategoryBottom", SlateBrush("GreyBase"));
	EditorStyle->Set("DetailsView.AdvancedDropdownBorder", SlateBrush("GreyBase"));
	EditorStyle->Set("DetailsView.AdvancedDropdownBorder.Open", SlateBrush("GreyBase"));

	UEditorStyleSettings* StyleSettings = GetMutableDefault<UEditorStyleSettings>();
	StyleSettings->SelectionColor = FLinearColor::FromSRGBColor(HoverBaseBright);
	StyleSettings->PressedSelectionColor = FLinearColor::FromSRGBColor(HoverBaseBright);
	StyleSettings->InactiveSelectionColor = FLinearColor::FromSRGBColor(HoverBase);
	StyleSettings->SaveConfig();

	FCoreStyle::SetSelectionColor(FLinearColor::FromSRGBColor(HoverBaseBright));
	//FCoreStyle::SetSelectorColor(FLinearColor::FromSRGBColor(Green));
	FCoreStyle::SetPressedSelectionColor(FLinearColor::FromSRGBColor(HoverBaseBright));
	FCoreStyle::SetInactiveSelectionColor(FLinearColor::FromSRGBColor(HoverBase));


	CoreStyle->Set("InvertedForeground", *BrushDatabase->GetSlateColor("TextColor"));
	CoreStyle->Set("DefaultForeground", *BrushDatabase->GetSlateColor("TextColor"));
	EditorStyle->Set("InvertedForeground", *BrushDatabase->GetSlateColor("TextColor"));
	EditorStyle->Set("DefaultForeground", *BrushDatabase->GetSlateColor("TextColor"));

	CoreStyle->Set(
		"Docking.UnhideTabwellButton",
		FButtonStyle(CoreStyle->GetWidgetStyle<FButtonStyle>("Button"))
		.SetNormal(ColorImageBrush(FColor::Transparent, FVector2D(10, 10)))
		.SetPressed(ImageBrush("/Docking/ShowTabwellButton_Pressed.png", FVector2D(10, 10)))
		.SetHovered(ImageBrush("/Docking/ShowTabwellButton_Hovered.png", FVector2D(10, 10)))
		.SetNormalPadding(0)
		.SetPressedPadding(0)
	);

	const FTableColumnHeaderStyle TableColumnHeaderStyle =
		FTableColumnHeaderStyle()
		.SetSortPrimaryAscendingImage(ImageBrush("/Panel/SortUpArrow.png", FVector2D(8, 4)))
		.SetSortPrimaryDescendingImage(ImageBrush("/Panel/SortDownArrow.png", FVector2D(8, 4)))
		.SetSortSecondaryAscendingImage(ImageBrush("/Panel/SortUpArrows.png", FVector2D(16, 4)))
		.SetSortSecondaryDescendingImage(ImageBrush("/Panel/SortDownArrows.png", FVector2D(16, 4)))
		.SetNormalBrush(*SlateBrush("GreyDark"))
		.SetHoveredBrush(*SlateBrush("HoverDark"))
		.SetMenuDropdownImage(ImageBrush("/Panel/ColumnHeader_Arrow.png", Icon8))
		.SetMenuDropdownNormalBorderBrush(*SlateBrush("GreyBase"))
		.SetMenuDropdownHoveredBorderBrush(*SlateBrush("HoverBase"));
	CoreStyle->Set("TableView.Header.Column", TableColumnHeaderStyle);

	const FSplitterStyle TableHeaderSplitterStyle =
		FSplitterStyle()
		.SetHandleNormalBrush(FSlateNoResource())
		.SetHandleHighlightBrush(ImageBrush("/Panel/HeaderSplitterGrip.png", Icon8));

	CoreStyle->Set(
		"TableView.Header",
		FHeaderRowStyle()
		.SetColumnStyle(TableColumnHeaderStyle)
		.SetLastColumnStyle(TableColumnHeaderStyle)
		.SetColumnSplitterStyle(TableHeaderSplitterStyle)
		.SetBackgroundBrush(*SlateBrush("GreyDark"))
		.SetForegroundColor(*BrushDatabase->GetSlateColor("TextColor"))
	);

	EditorStyle->Set(
		"PropertyTable.HeaderRow",
		FHeaderRowStyle()
		.SetColumnStyle(TableColumnHeaderStyle)
		.SetLastColumnStyle(TableColumnHeaderStyle)
		.SetColumnSplitterStyle(TableHeaderSplitterStyle)
		.SetBackgroundBrush(*SlateBrush("GreyDark"))
		.SetForegroundColor(*BrushDatabase->GetSlateColor("TextColor"))
	);

	CoreStyle->Set("PopupText.Background", SlateBrush("GreyDark"));
	CoreStyle->Set("NotificationList.ItemBackground", SlateBrush("PanelDark"));

	EditorStyle->Set(
		"TableView.DarkRow",
		FTableRowStyle(EditorStyle->GetWidgetStyle<FTableRowStyle>("TableView.Row"))
		.SetEvenRowBackgroundBrush(*SlateBrush("GreyBase"))
		.SetEvenRowBackgroundHoveredBrush(*SlateBrush("GreyBase"))
		.SetOddRowBackgroundBrush(*SlateBrush("GreyBase"))
		.SetOddRowBackgroundHoveredBrush(*SlateBrush("GreyBase"))
		.SetSelectorFocusedBrush(*SlateBrush("HoverBase"))
		.SetActiveBrush(*SlateBrush("HoverBase"))
		.SetActiveHoveredBrush(*SlateBrush("HoverBase"))
		.SetInactiveBrush(*SlateBrush("HoverBase"))
		.SetInactiveHoveredBrush(*SlateBrush("HoverBase"))
	);

	EditorStyle->Set(
		"PlacementBrowser.Tab",
		FCheckBoxStyle()
		.SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
		.SetUncheckedImage(*SlateBrush("GreyDark"))
		.SetUncheckedPressedImage(*SlateBrush("HoverDark"))
		.SetUncheckedHoveredImage(*SlateBrush("HoverDark"))
		.SetCheckedImage(*SlateBrush("HoverDark"))
		.SetCheckedHoveredImage(*SlateBrush("HoverDark"))
		.SetCheckedPressedImage(*SlateBrush("HoverDark"))
		.SetPadding(0));

	EditorStyle->Set("Sequencer.ToolBar.Background", SlateBrush("GreyBase"));
}
