// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class ISlateStyle;
class FSlateStyleSet;

/**
* styles for level stream assistant plugin
*/
class FLevelStreamAssistantStyle
{
public:

	static void Initialize();

	static void Shutdown();

	LEVELSTREAMASSISTANT_API static FName GetStyleSetName();

	LEVELSTREAMASSISTANT_API static const ISlateStyle& Get();

	/** reloads textures used by slate renderer */
	static void ReloadTextures();

	// @return attribute that gives black color when button is hovered and white when it's not.
	FORCEINLINE static TAttribute<FSlateColor> GetButtonForegroundAttribute(TSharedPtr<class SButton> Button)
	{
		return TAttribute<FSlateColor>::Create(
			TAttribute<FSlateColor>::FGetter::CreateStatic(
				&FLevelStreamAssistantStyle::GetButtonForegroundColor, Button));
	}

	FORCEINLINE static TAttribute<FSlateColor> GetComboButtonForegroundAttribute(TSharedPtr<class SComboButton> Button)
	{
		return TAttribute<FSlateColor>::Create(
			TAttribute<FSlateColor>::FGetter::CreateStatic(
				&FLevelStreamAssistantStyle::GetComboButtonForegroundColor, Button));
	}

private:
	static FSlateColor GetButtonForegroundColor(TSharedPtr<class SButton> Button);
	static FSlateColor GetComboButtonForegroundColor(TSharedPtr<class SComboButton> Button);

	static TSharedRef<FSlateStyleSet> Create();

	static void HandleNullInstance();

	static TSharedPtr<FSlateStyleSet> StyleInstance;
};