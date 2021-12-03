// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "SceneHighlights.h"
#include <Utilities/LevelStreamAssistantUtils.h>
#include <Utilities/LevelStreamAssistantUtilsDrawing.h>
#include <Engine/LevelStreaming.h>

namespace LevelStreamAssistant
{
	void FSceneHighlights::HighlightLevel(TWeakObjectPtr<ULevelStreaming> LevelStreaming, const FLinearColor& Color, bool bBlink)
	{
		if (LevelStreaming.IsValid() && LevelStreaming->IsLevelLoaded())
		{
			ULevel* LevelToHighlight = LevelStreaming->GetLoadedLevel();
			FBox BoxToHighlight = LevelStreamAssistantUtils::CalculateLevelBounds(LevelToHighlight, true);

			FColorFunction ColorFunc = bBlink ? BlinkColor(IdentityColor(Color)) : IdentityColor(Color);
			FRenderFunction RenderFunc = BoxHighlighter(BoxToHighlight, FTransform(FVector(BoxToHighlight.GetCenter())));
			AddHighlight(LevelStreaming, RenderFunc, ColorFunc);
		}
	}

	void FSceneHighlights::HighlightActor(TWeakObjectPtr<AActor> ActorToHighlight, const FLinearColor& Color, bool bBlink)
	{
		FColorFunction ColorFunc = bBlink ? BlinkColor(IdentityColor(Color)) : IdentityColor(Color);
		
		if (ABrush* BrushToHighlight = Cast<ABrush>(ActorToHighlight))
		{
			FRenderFunction RenderFunc = BrushHighlighter(BrushToHighlight);
			AddHighlight(ActorToHighlight, RenderFunc, ColorFunc);
		}
		else if (ActorToHighlight.IsValid())
		{
			FBox BoxToHighlight = LevelStreamAssistantUtils::CalculateActorLocalBounds(ActorToHighlight.Get(), true);
			FRenderFunction RenderFunc = BoxHighlighter(BoxToHighlight, ActorToHighlight->GetActorTransform());
			AddHighlight(ActorToHighlight, RenderFunc, ColorFunc);
		}
	}

	void FSceneHighlights::AddHighlight(TWeakObjectPtr<UObject> KeyObject, const FRenderFunction& RenderFunc, const FColorFunction& ColorFunc)
	{
		FHighlightInfo Highlight(ColorFunc, RenderFunc);
		HighlightMap.Add(KeyObject, Highlight);
	}

	void FSceneHighlights::RemoveHighlight(TWeakObjectPtr<UObject> Key)
	{
		HighlightMap.Remove(Key);
	}

	void FSceneHighlights::ClearHighlights()
	{
		HighlightMap.Empty(4);
	}

	FSceneHighlights::FRenderFunction FSceneHighlights::BrushHighlighter(TWeakObjectPtr<ABrush> Brush) const
	{
		return FRenderFunction([WeakBrushPtr = Brush](const FSceneView& View, FPrimitiveDrawInterface& PDI, FColorFunction Color)
		{
			if (ABrush* Brush = WeakBrushPtr.Get())
			{
				LevelStreamAssistantUtils::RenderPoly(*Brush, Color(), View, PDI);
			}
		});
	}

	FSceneHighlights::FRenderFunction FSceneHighlights::BoxHighlighter(const FBox& Box, const FTransform& Transform) const
	{
		return FRenderFunction([Box, Transform](const FSceneView& View, FPrimitiveDrawInterface& PDI, FColorFunction Color)
		{
			if (Box.IsValid)
			{
				LevelStreamAssistantUtils::DrawBox2(Transform.ToMatrixWithScale(), Box.GetExtent(), Color(), View, PDI);
			}
		});
	}

	FSceneHighlights::FColorFunction FSceneHighlights::IdentityColor(const FLinearColor& Color) const
	{
		return FColorFunction([Color]()
		{
			return Color;
		});
	}

	FSceneHighlights::FColorFunction FSceneHighlights::BlinkColor(FColorFunction ColorFunction) const
	{
		return FColorFunction([ColorFunction]()
		{
			FLinearColor Color = ColorFunction();
			double Seconds = FDateTime::UtcNow().GetTimeOfDay().GetTotalSeconds();
			float Amplitude = 0.1f;
			float PulsatingAlpha = FMath::Sin(Seconds * PI * 2. * UE_GOLDEN_RATIO) * 0.5f + 0.5f;
			float Opacity = FMath::Lerp(Color.A - Amplitude, Color.A + Amplitude, PulsatingAlpha);
			return Color.CopyWithNewOpacity(Opacity);
		});
	}

	void FSceneHighlights::Render(const FSceneView& View, FPrimitiveDrawInterface& PDI) const
	{
		for (TPair<TWeakObjectPtr<UObject>, FHighlightInfo> Highlight : HighlightMap)
		{
			Highlight.Value.Render(View, PDI);
		}
	}

	void FSceneHighlights::FHighlightInfo::Render(const FSceneView& View, FPrimitiveDrawInterface& PDI) const
	{
		if(ensure(RenderFunction && ColorFunction)) RenderFunction(View, PDI, ColorFunction);
	}
}