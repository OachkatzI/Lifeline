// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Assistant/STreeViewBase.h>
#include <Widgets/Views/STableRow.h>
#include "Widgets/DeclarativeSyntaxSupport.h"

class ITableRow;
class FUICommandInfo;
class FExtender;
namespace EExtensionHook { enum Position; }

namespace LevelStreamAssistant
{
	struct FBaseItem;
	class FActorIconFinder;
	class ILevelStreamingEditor;
	class FItemContextMenuModifier;

	class STreeViewActors : public STreeViewBase
	{
	public:
		DECLARE_DELEGATE(FOnSelectionChanged);

		SLATE_BEGIN_ARGS(STreeViewActors) :
			_TreeItemsSource(nullptr)
		{ }

		SLATE_ARGUMENT(const TArray<TSharedPtr<FBaseItem>>*, TreeItemsSource)

		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, TSharedRef<ILevelStreamingEditor> InLevelStreamingEditor);

		~STreeViewActors();

	private:
		TSharedRef<SWidget> OnGenerateNameWidget(TSharedRef<ITableRow> Row, TSharedPtr<FBaseItem> InItem);
		STableRow<TSharedPtr<FBaseItem>>::FArguments GetTableRowArgs(TSharedPtr<FBaseItem> InItem);
		
		void OnItemRenamed(const FText& NewName, TSharedPtr<FBaseItem> InItem) const;

		void OnSelectionChanged(TSharedPtr<FBaseItem> Entry, ESelectInfo::Type SelectInfo);

		TSharedPtr<SWidget> OnItemContextMenuOpening(TSharedPtr<FBaseItem> Item);
		bool ContextMenuAppliesToAllItems(const FItemContextMenuModifier& ContextMenuModifier, const TArray<TSharedPtr<FBaseItem>>& Items) const;

		void BindCommands();
		void PopulateContextMenuModifiers();
		void BuildContextMenuForLevelStreamingEditor(FExtender& MenuExtender) const;
		void ExtendMenuEntry(FExtender& Extender, const FName& Hook, EExtensionHook::Position Position, TSharedPtr<FUICommandInfo> CommandInfo);

		void CreateFolder();
		void CreateSubFolder();
		void SetFoldersNewColor();
		void RenameItems();
		void DeleteItems();

		virtual FFolderManager& GetFolderManager() const override;

		void OnSelectedActorsChanged();

		bool bSelectionChangeInternal = false;
		bool bSelectionChangeExternal = false;

		TArray<TSharedPtr<const FItemContextMenuModifier>> ContextMenuModifiers;

		// used to cache icons for actor rows.
		TSharedPtr<FActorIconFinder> ActorIconFinder;

		TSharedPtr<ILevelStreamingEditor> LevelStreamingEditor;
	};
}