// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class AActor;
class ULevelStreaming;

// interface used by LevelStreamAssistant to make or destroy relations between actors and levels.
// binding modes provide a way to distinguish between different types of level streaming actions.
// this class should remain stateless.
class LEVELSTREAMASSISTANT_API FLevelStreamBindingMode
{
public:
	// set target actor to stream a level. (e.g. to load or unload the level)
	virtual void Set(AActor& Target, ULevelStreaming& LevelStreaming) const = 0;

	// clear target actor from streaming a level. (i.e. the actor should no longer stream the level)
	virtual void Clear(AActor& Target, ULevelStreaming& LevelStreaming) const = 0;

	// check if target actor is set to stream a level.
	// NOTICE: return value must agree with Set/Clear definition.
	virtual bool IsSet(const AActor& Target, const ULevelStreaming& LevelStreaming) const = 0;
	
	// get tooltip for this mode.
	virtual FText GetToolTip() const;

	// get color for this mode. this color is also used for highlighting levels in the viewport. opacity is not used.
	virtual FLinearColor GetColor() const;
};
