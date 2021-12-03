// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Widgets/SCompoundWidget.h>
#include <Widgets/DeclarativeSyntaxSupport.h>

class ITableRow;
class AActor;
class ULevelStreaming;

namespace LevelStreamAssistant
{
	struct FBaseItem;

	class FActorIconFinder;

	// an icon for FBaseItem.
	class SItemIcon : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SItemIcon) { }
		SLATE_ARGUMENT(TSharedPtr<FActorIconFinder>, ActorIconFinder)
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, const TSharedRef<ITableRow>& InRow, const TSharedRef<FBaseItem>& InItem);

	private:

		const FSlateBrush* GetActorIcon(TWeakObjectPtr<AActor> Actor) const;
		const FSlateBrush* GetLevelIcon(TWeakObjectPtr<ULevelStreaming> Level) const;
		const FSlateBrush* GetFolderIcon() const;
		
		FSlateColor GetFolderColor(TSharedPtr<const FBaseItem> FolderItem) const;

		TWeakPtr<ITableRow> Row;
		TSharedPtr<FActorIconFinder> ActorIconFinder;
	};
}
