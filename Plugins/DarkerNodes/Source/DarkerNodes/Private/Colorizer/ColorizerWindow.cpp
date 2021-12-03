/* Copyright (C) 2021 Hugo ATTAL - All Rights Reserved
* This plugin is downloadable from the Unreal Engine Marketplace
*/

#include "Colorizer.h"
#include "Classes/EditorStyleSettings.h"

void FColorizer::ColorWindow()
{
	if (!DarkerNodesSettings->UseCustomPanels)
	{
		return;
	}
	
#if !PLATFORM_MAC
	const FVector2D IconWindow = FVector2D(30, 18);
	const FColor TextColor50 = TextColor.WithAlpha(255 * 0.5);
	const FColor TextColor75 = TextColor.WithAlpha(255 * 0.75);

	const FButtonStyle Button = CoreStyle->GetWidgetStyle<FButtonStyle>("Button");

	const FButtonStyle MinimizeButtonStyle =
		FButtonStyle(Button)
		.SetNormal(*CenterIcon("WindowButton_Minimize_Normal", "/Window/WindowButton_Minimize.png", IconWindow, TextColor50))
		.SetHovered(*CenterIcon("WindowButton_Minimize_Hovered", "/Window/WindowButton_Minimize.png", IconWindow, TextColor))
		.SetPressed(*CenterIcon("WindowButton_Minimize_Pressed", "/Window/WindowButton_Minimize.png", IconWindow, TextColor75));

	const FButtonStyle MaximizeButtonStyle =
		FButtonStyle(Button)
		.SetNormal(*CenterIcon("WindowButton_Maximize_Normal", "/Window/WindowButton_Maximize.png", IconWindow, TextColor50))
		.SetHovered(*CenterIcon("WindowButton_Maximize_Hovered", "/Window/WindowButton_Maximize.png", IconWindow, TextColor))
		.SetPressed(*CenterIcon("WindowButton_Maximize_Pressed", "/Window/WindowButton_Maximize.png", IconWindow, TextColor75));

	const FButtonStyle RestoreButtonStyle =
		FButtonStyle(Button)
		.SetNormal(*CenterIcon("WindowButton_Restore_Normal", "/Window/WindowButton_Restore.png", IconWindow, TextColor50))
		.SetHovered(*CenterIcon("WindowButton_Restore_Hovered", "/Window/WindowButton_Restore.png", IconWindow, TextColor))
		.SetPressed(*CenterIcon("WindowButton_Restore_Pressed", "/Window/WindowButton_Restore.png", IconWindow, TextColor75));

	const FButtonStyle CloseButtonStyle =
		FButtonStyle(Button)
		.SetNormal(*CenterIcon("WindowButton_Close_Normal", "/Window/WindowButton_Close.png", IconWindow, TextColor50))
		.SetHovered(*CenterIcon("WindowButton_Close_Hovered", "/Window/WindowButton_Close.png", IconWindow, TextColor))
		.SetPressed(*CenterIcon("WindowButton_Close_Pressed", "/Window/WindowButton_Close.png", IconWindow, TextColor75));
#endif


	const FTextBlockStyle TitleTextStyle =
		FTextBlockStyle(CoreStyle->GetWidgetStyle<FTextBlockStyle>("NormalText"))
		.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", 9))
		.SetShadowOffset(FVector2D(1.0f, 1.0f));

	CoreStyle->Set(
		"Window",
		FWindowStyle()
#if !PLATFORM_MAC
		.SetMinimizeButtonStyle(MinimizeButtonStyle)
		.SetMaximizeButtonStyle(MaximizeButtonStyle)
		.SetRestoreButtonStyle(RestoreButtonStyle)
		.SetCloseButtonStyle(CloseButtonStyle)
#endif
		.SetTitleTextStyle(TitleTextStyle)
		.SetActiveTitleBrush(ImageBrush("/Transparent.png", Icon32))
		.SetInactiveTitleBrush(ImageBrush("/Window/WindowTitle_Inactive.png", Icon32))
		.SetFlashTitleBrush(ImageBrush("/Window/WindowTitle_Flashing.png", Icon24))
		.SetOutlineBrush(BorderBrush("/Window/WindowOutline.png"))
		.SetOutlineColor(FLinearColor(0.1f, 0.1f, 0.1f, 1.0f))
		.SetBorderBrush(BoxBrush("/Transparent.png", Icon256, 0.48f))
		.SetBackgroundBrush(*SlateBrush("MainWindowColor"))
		.SetChildBackgroundBrush(*SlateBrush("ChildWindowColor"))
	);
}
