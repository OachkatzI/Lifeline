// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FPrimitiveDrawInterface;
class FSceneView;

class ULevelStreaming;
class AActor;
class ABrush;

namespace LevelStreamAssistant
{
	class FSceneHighlights
	{
	public:
		void HighlightLevel(TWeakObjectPtr<ULevelStreaming> LevelStreaming, const FLinearColor& Color, bool bBlink = false);
		void HighlightActor(TWeakObjectPtr<AActor> Actor, const FLinearColor& Color, bool bBlink = false);

		void RemoveHighlight(TWeakObjectPtr<UObject> Key);
		void ClearHighlights();

		void Render(const FSceneView& View, FPrimitiveDrawInterface& PDI) const;

	private:
		typedef TFunction<FLinearColor()> FColorFunction;
		typedef TFunction<void(const FSceneView&, FPrimitiveDrawInterface&, FColorFunction)> FRenderFunction;

		struct FHighlightInfo
		{
			FHighlightInfo(const FColorFunction& ColorFunction, const FRenderFunction& RenderFunction)
				: ColorFunction(ColorFunction), RenderFunction(RenderFunction) { }

			FColorFunction ColorFunction;
			FRenderFunction RenderFunction;

			void Render(const FSceneView& View, FPrimitiveDrawInterface& PDI) const;
		};

	private:
		void AddHighlight(TWeakObjectPtr<UObject> KeyObject, const FRenderFunction& RenderFunc, const FColorFunction& ColorFunc);

		FRenderFunction BrushHighlighter(TWeakObjectPtr<ABrush> Brush) const;
		FRenderFunction BoxHighlighter(const FBox& Box, const FTransform& Transform) const;
		FColorFunction IdentityColor(const FLinearColor& Color) const;
		FColorFunction BlinkColor(FColorFunction ColorFunction) const;
		
		TMap<TWeakObjectPtr<UObject>, FHighlightInfo> HighlightMap;
	};
}
