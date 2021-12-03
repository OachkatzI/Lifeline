#pragma once

// #include "IPluginManager.h"
#include "Brushes/SlateImageBrush.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BOX_BRUSH(RelativePath, ...) FSlateBoxBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BORDER_BRUSH(RelativePath, ...) FSlateBorderBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define TTF_FONT(RelativePath, ...) FSlateFontInfo(RootToContentDir(RelativePath, TEXT(".ttf")), __VA_ARGS__)
#define OTF_FONT(RelativePath, ...) FSlateFontInfo(RootToContentDir(RelativePath, TEXT(".otf")), __VA_ARGS__)


/**
 * Implements the visual style of the Atom editor UI.
 */
class FUnrealAtomsEditorStyle
	: public FSlateStyleSet
{
public:

	/** Default constructor. */
	FUnrealAtomsEditorStyle()
		: FSlateStyleSet("UnrealAtomsEditorStyle")
	{
		const FVector2D Icon16x16(16.0f, 16.0f);
		const FVector2D Icon32x32(32.0f, 32.0f);
		const FVector2D Icon64x64(64.0f, 64.0f);
		const FVector2D Icon128x128(128.0f, 128.0f);

		FString RootDirectory;

		auto ProjectPluginDir = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectPluginsDir() + "UnrealAtoms"));
		auto EnginePluginDir  =
			FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::EnginePluginsDir() + TEXT("UnrealAtoms")));

		if (FPaths::DirectoryExists(ProjectPluginDir))
		{
			RootDirectory = ProjectPluginDir;
		}
			// Is it in the engines marketplace folder ?
		else if (FPaths::DirectoryExists(EnginePluginDir))
		{
			RootDirectory = EnginePluginDir;
		}
		SetContentRoot(RootDirectory);

		// set new styles here, for example...
		// Set("ClassThumbnail.AtomBaseVariable", new IMAGE_BRUSH(TEXT("Resources/IconAtomVariablex64"), Icon64x64));
		// Set("ClassIcon.AtomBaseVariable", new IMAGE_BRUSH(TEXT("Resources/IconAtomVariablex16"), Icon16x16));

		Set("ClassThumbnail.AtomBaseVariable", new IMAGE_BRUSH(TEXT("Resources/IconAtomVariable_x64"), Icon64x64));
		Set("ClassIcon.AtomBaseVariable", new IMAGE_BRUSH(TEXT("Resources/IconAtomVariable_x16"), Icon16x16));

		Set("ClassThumbnail.AtomBaseEvent", new IMAGE_BRUSH(TEXT("Resources/IconAtomEvent_x64"), Icon64x64));
		Set("ClassIcon.AtomBaseEvent", new IMAGE_BRUSH(TEXT("Resources/IconAtomEvent_x16"), Icon16x16));
		FSlateStyleRegistry::RegisterSlateStyle(*this);
	}


	/** Destructor. */
	~FUnrealAtomsEditorStyle()
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*this);
	}
};


#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT
