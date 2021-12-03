// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "LevelStreamAssistantStyle.h"
#include "Styling/SlateStyle.h"
#include "LevelStreamAssistantLog.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Interfaces/IPluginManager.h"
#include "EditorStyleSet.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboButton.h"

TSharedPtr<FSlateStyleSet> FLevelStreamAssistantStyle::StyleInstance = NULL;

void FLevelStreamAssistantStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FLevelStreamAssistantStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FLevelStreamAssistantStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("LevelStreamAssistantStyle"));
	return StyleSetName;
}

const ISlateStyle& FLevelStreamAssistantStyle::Get()
{
	if (!ensure(StyleInstance.IsValid()))
	{
		HandleNullInstance();
	}

	return *StyleInstance;
}

void FLevelStreamAssistantStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		if (FSlateRenderer* SlateRenderer = FSlateApplication::Get().GetRenderer())
		{
			SlateRenderer->ReloadTextureResources();
		}
	}
}

FSlateColor FLevelStreamAssistantStyle::GetButtonForegroundColor(TSharedPtr<SButton> Button)
{
	return Button.IsValid() && (Button->IsHovered() || Button->IsPressed())
		? FLevelStreamAssistantStyle::Get().GetSlateColor("LSA.TextBlock.Black")
		: FLevelStreamAssistantStyle::Get().GetSlateColor("LSA.TextBlock.White");
}

FSlateColor FLevelStreamAssistantStyle::GetComboButtonForegroundColor(TSharedPtr<SComboButton> Button)
{
	return Button.IsValid() && (Button->IsHovered())
		? FLevelStreamAssistantStyle::Get().GetSlateColor("LSA.TextBlock.Black")
		: FLevelStreamAssistantStyle::Get().GetSlateColor("LSA.TextBlock.White");
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

#define IMAGE_BRUSH_CORE( RelativePath, ... ) FSlateImageBrush( Style->RootToCoreContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

TSharedRef<FSlateStyleSet> FLevelStreamAssistantStyle::Create()
{
	const FVector2D Icon8x8(8.0f, 8.0f);
	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon24x24(24.0f, 24.0f);
	const FVector2D Icon32x32(32.0f, 32.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);

	const FLinearColor Blue(0.11f, 0.215f, 0.401f);
	const FLinearColor BlueDark(0.094f, 0.195f, 0.401f);
	const FSlateColor WhiteText(FLinearColor(0.72f, 0.72f, 0.72f, 1.f));
	const FSlateColor BlackText(FLinearColor(0.0f, 0.0f, 0.0f, 1.f));
	
	const FLinearColor Orange(0.728f, 0.364f, 0.003f);
	const FLinearColor OrangeDark(0.701f, 0.225f, 0.003f);
	const FLinearColor BlueHighlight(0.094f, 0.195f, 0.401f, 0.5f);

	const FLinearColor Grey(FLinearColor(0.024158f, 0.024158f, 0.024158f, 1.f)* 1.67252838f);

	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("LevelStreamAssistant")->GetBaseDir() / TEXT("Resources"));
	Style->SetCoreContentRoot(FPaths::EngineContentDir());

	Style->Set("LSA.Commands.Settings", new IMAGE_BRUSH(TEXT("icon_settings"), Icon24x24));
	Style->Set("LSA.Commands.Option_ShowOnlyRelatedLevels", new IMAGE_BRUSH(TEXT("icon_isolate"), Icon24x24));
	Style->Set("LSA.Commands.Option_HighlightRelatedLevels", new IMAGE_BRUSH(TEXT("icon_highlight"), Icon24x24));
	Style->Set("LSA.Commands.Option_FocusSelectedActor", new IMAGE_BRUSH(TEXT("icon_focus_trigger"), Icon24x24));
	Style->Set("LSA.Commands.Option_FocusBoundLevels", new IMAGE_BRUSH(TEXT("icon_focus_level"), Icon24x24));
	Style->Set("LSA.Commands.Option_SyncWithViewportSelection", new IMAGE_BRUSH(TEXT("icon_sync_selection"), Icon24x24));
	Style->Set("LSA.Commands.CreateFolder", new IMAGE_BRUSH(TEXT("icon_AddFolder_16x"), Icon24x24));
	Style->Set("LSA.Commands.CreateSubFolder", new IMAGE_BRUSH(TEXT("icon_SubFolder_16x"), Icon24x24));
	Style->Set("LSA.Commands.FindLevelsInContentBrowser", new IMAGE_BRUSH_CORE("Editor/Slate/Icons/lens_12x", Icon24x24));

	Style->Set("LSA.Logo.Large", new IMAGE_BRUSH(TEXT("Icon128"), Icon40x40));
	Style->Set("LSA.Logo.Medium", new IMAGE_BRUSH(TEXT("Icon128"), Icon20x20));
	Style->Set("LSA.Logo.Small", new IMAGE_BRUSH(TEXT("Icon128"), Icon16x16));

	Style->Set("LSA.Size.Disk", new IMAGE_BRUSH(TEXT("Disk"), Icon16x16));
	Style->Set("LSA.Size.Memory", new IMAGE_BRUSH(TEXT("Ram"), Icon16x16));

	Style->Set("LSA.Level.Visible", new IMAGE_BRUSH(TEXT("Eye_open"), Icon16x16));
	Style->Set("LSA.Level.Hidden", new IMAGE_BRUSH(TEXT("Eye_close"), Icon16x16));

	Style->Set("LSA.Tools.LevelColoration", new IMAGE_BRUSH(TEXT("level_coloration"), Icon40x40));
	Style->Set("LSA.Tools.Play", new IMAGE_BRUSH(TEXT("Play"), Icon40x40));
	Style->Set("LSA.Tools.LevelStreamAssistantTab", new IMAGE_BRUSH(TEXT("LevelStreamAssistantTab"), Icon40x40));
	Style->Set("LSA.Tools.ActorSelectorBox", new IMAGE_BRUSH(TEXT("ActorVolumeSelector"), Icon32x32));

	Style->Set("LSA.Level.Zone.Blue", new IMAGE_BRUSH(TEXT("Box_Blue"), Icon32x32));
	Style->Set("LSA.Level.Zone.Green", new IMAGE_BRUSH(TEXT("Box_Green"), Icon32x32));
	//Style->Set("LSA.Level.Zone.Yellow", new IMAGE_BRUSH(TEXT("Box_Yellow"), Icon32x32));
	Style->Set("LSA.Level.Zone.Orange", new IMAGE_BRUSH(TEXT("Box_Orange"), Icon32x32));
	//Style->Set("LSA.Level.Zone.Red", new IMAGE_BRUSH(TEXT("Box_Red"), Icon32x32));

	Style->Set("LSA.Binding.Unset", new IMAGE_BRUSH(TEXT("Trigger"), Icon32x32));

	Style->Set("LSA.Binding.Undetermined", new IMAGE_BRUSH(TEXT("TriggerMultipleValues"), Icon32x32));
	
	Style->Set("LSA.NewFolderIcon", new IMAGE_BRUSH("icon_AddFolder_16x", Icon16x16));

	const FTableRowStyle& DefaultTableRowStyle = FEditorStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row");
	Style->Set("LSA.ListViewActor.Row", FTableRowStyle(DefaultTableRowStyle)
		.SetActiveBrush(IMAGE_BRUSH_CORE("Slate/Common/Selection", Icon8x8, BlueDark))
		.SetInactiveBrush(IMAGE_BRUSH_CORE("Slate/Common/Selection", Icon8x8, BlueDark))
		.SetActiveHoveredBrush(IMAGE_BRUSH_CORE("Slate/Common/Selection", Icon8x8, Blue))
		.SetInactiveHoveredBrush(IMAGE_BRUSH_CORE("Slate/Common/Selection", Icon8x8, Blue))
	);
	
	Style->Set("LSA.ListViewLevel.Row", FTableRowStyle(DefaultTableRowStyle)
		.SetActiveBrush(IMAGE_BRUSH_CORE("Slate/Common/Selection", Icon8x8, OrangeDark))
		.SetInactiveBrush(IMAGE_BRUSH_CORE("Slate/Common/Selection", Icon8x8, OrangeDark))
		.SetActiveHoveredBrush(IMAGE_BRUSH_CORE("Slate/Common/Selection", Icon8x8, Orange))
		.SetInactiveHoveredBrush(IMAGE_BRUSH_CORE("Slate/Common/Selection", Icon8x8, Orange))
		.SetActiveHighlightedBrush(IMAGE_BRUSH_CORE("Slate/Common/Selection", Icon8x8, BlueHighlight))
		.SetInactiveHighlightedBrush(IMAGE_BRUSH_CORE("Slate/Common/Selection", Icon8x8, BlueHighlight))
	);

	Style->Set("InvisibleButton", FButtonStyle()
		.SetNormal(FSlateNoResource())
		.SetHovered(FSlateNoResource())
		.SetPressed(FSlateNoResource())
		.SetDisabled(FSlateNoResource())
	);

	Style->Set("LSA.TextBlock.White", WhiteText);
	Style->Set("LSA.TextBlock.Black", BlackText);

	FSearchBoxStyle DefaultSearchBoxStyle = FCoreStyle::Get().GetWidgetStyle<FSearchBoxStyle>("SearchBox");
	Style->Set("LSA.ListView.SearchBox", FSearchBoxStyle(DefaultSearchBoxStyle)
		.SetImagePadding(FMargin(4.0f))
		.SetTextBoxStyle(DefaultSearchBoxStyle.TextBoxStyle
			.SetBackgroundColor(Grey)
			.SetForegroundColor(WhiteText)
		)
	);

	Style->Set("ClassIcon.ActorSelectorBox", new IMAGE_BRUSH(TEXT("ActorVolumeSelector"), Icon16x16));
	Style->Set("ClassIcon.LevelStreamingTrigger", new IMAGE_BRUSH_CORE(TEXT("Editor/Slate/Icons/AssetIcons/TriggerBase_16x"), Icon16x16));

	return Style;
}

FORCENOINLINE void FLevelStreamAssistantStyle::HandleNullInstance()
{
	// if style instance is null, try to re-initialize and prevent editor from crash.
	// this should never happen but it's here just in case.
	FSlateStyleRegistry::UnRegisterSlateStyle(GetStyleSetName());
	StyleInstance = Create();
	FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	UE_LOG(LogLevelStreamAssistant, Error, TEXT("Style instance was invalid! a new one was created."));
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT
#undef IMAGE_BRUSH_CORE
