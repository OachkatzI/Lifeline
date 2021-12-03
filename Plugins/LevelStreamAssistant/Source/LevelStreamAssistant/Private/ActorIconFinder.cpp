// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "ActorIconFinder.h" 
#include <GameFramework/Actor.h>
#include <ClassIconFinder.h>
#include <Styling/SlateBrush.h>

namespace LevelStreamAssistant
{
	const FSlateBrush* FActorIconFinder::FindActorIcon(TWeakObjectPtr<AActor> InActor)
	{
		if (AActor* Actor = InActor.Get())
		{
			FName IconName = Actor->GetCustomIconName();
			if (IconName == NAME_None)
			{
				IconName = *Actor->GetClass()->GetFullName();
			}

			if (TSharedPtr<FSlateBrush>* Icon = Cache.Find(IconName))
			{
				// return cached icon
				return (*Icon).Get();
			}
			else if (const FSlateBrush* FoundIcon = FClassIconFinder::FindIconForActor(Actor))
			{
				// cache found icon and return
				Cache.Add(IconName, MakeShareable(new FSlateBrush(*FoundIcon)));
				return FoundIcon;
			}
		}
		return nullptr;
	}
}
