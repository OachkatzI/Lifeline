// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FSlateBrush;

namespace LevelStreamAssistant
{
	class FActorIconFinder
	{
	public:
		const FSlateBrush* FindActorIcon(TWeakObjectPtr<AActor> InActor);

	private:
		TMap<FName, TSharedPtr<FSlateBrush>> Cache;
	};
}