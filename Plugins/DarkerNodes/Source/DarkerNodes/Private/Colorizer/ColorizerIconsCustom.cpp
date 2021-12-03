/* Copyright (C) 2021 Hugo ATTAL - All Rights Reserved
* This plugin is downloadable from the Unreal Engine Marketplace
*/

#include "Colorizer.h"
#include "Styling/SlateStyleRegistry.h"

void FColorizer::ColorIconsCustom()
{
	if (DarkerNodesSettings->UseCustomIcons)
	{
		const ISlateStyle* MegascansConstStyle = FSlateStyleRegistry::FindSlateStyle("MegascansStyle");
		if (MegascansConstStyle != nullptr)
		{
			FSlateStyleSet* MegascansStyle = static_cast<FSlateStyleSet*>(const_cast<ISlateStyle*>(MegascansConstStyle));
			AddMenuIcon(MegascansStyle, "Megascans.Logo", "/Menu/megascans.png", ImageColor);
		}
	}
}	