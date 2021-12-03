// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Assistant/Folders/FolderManager.h" 
#include <Assistant/Folders/HierarchyManagerObject.h>
#include <ScopedTransaction.h>
#include <LevelStreamAssistantLog.h>

#define LOCTEXT_NAMESPACE "LevelStreamAssistant.FFolderManager"

namespace LevelStreamAssistant
{
	FFolderManager* FFolderManager::ActorFolderManager = nullptr;
	FFolderManager* FFolderManager::LevelFolderManager = nullptr;

	FFolderManager::FFolderManager(bool bIsForLevels)
		: bIsForLevels(bIsForLevels)
	{
	}

	FFolderManager::~FFolderManager()
	{
	}

	FFolderManager& FFolderManager::GetActorFolderManager()
	{
		if (!ensure(ActorFolderManager)) Initialize();
		return *ActorFolderManager;
	}

	FFolderManager& FFolderManager::GetLevelFolderManager()
	{
		if (!ensure(LevelFolderManager)) Initialize();
		return *LevelFolderManager;
	}

	void FFolderManager::Initialize()
	{
		if (!ActorFolderManager)
		{
			ActorFolderManager = new FFolderManager(/*bIsForLevels*/false);
		}
		
		if (!LevelFolderManager)
		{
			LevelFolderManager = new FFolderManager(/*bIsForLevels*/true);
		}
	}

	void FFolderManager::Shutdown()
	{
		if (ActorFolderManager)
		{
			delete ActorFolderManager;
			ActorFolderManager = nullptr;
		}

		if (LevelFolderManager)
		{
			delete LevelFolderManager;
			LevelFolderManager = nullptr;
		}
	}

	FName FFolderManager::GetFolderPathOf(UWorld& InWorld, TWeakObjectPtr<const UObject> InObject)
	{
		if (const UObject* Object = InObject.Get())
		{
			return GetHierarchyFor(InWorld).GetFolderPathOf(*Object);
		}
		return NAME_None;
	}

	void FFolderManager::SetFolderPathTo(UWorld& InWorld, TWeakObjectPtr<const UObject> InObject, FName NewPath)
	{
		if (const UObject* Object = InObject.Get())
		{
			FScopedTransaction Transaction(LOCTEXT("TransactionMoveObjectToFolder", "Move Object To Folder"));

			ModifyHierarchy(InWorld);

			FHierarchyInfo& Hierarchy = GetHierarchyFor(InWorld);
			FName OldPath = Hierarchy.GetFolderPathOf(*Object);
			Hierarchy.SetFolderPathFor(*Object, NewPath);

			OnObjectFolderChanged.Broadcast(Object, OldPath, NewPath);
		}
	}

	FFolderProperties* FFolderManager::GetFolderProperties(UWorld& InWorld, const FName& FolderName)
	{
		return GetHierarchyFor(InWorld).Folders.Find(FolderName);
	}

	TArray<FName> FFolderManager::GetAllFolders(UWorld& InWorld)
	{
		TArray<FName> AllFolders;
		GetHierarchyFor(InWorld).Folders.GenerateKeyArray(AllFolders);
		return AllFolders;
	}

	FName FFolderManager::GetUniqueFolderName(UWorld& InWorld, const FName& ParentFolder, const FName& BaseName)
	{
		FHierarchyInfo& Hierarchy = GetHierarchyFor(InWorld);

		FString ParentFolderStr = ParentFolder.IsNone() ? TEXT("") : ParentFolder.ToString();

		for (uint32 Index = 0; Index < TNumericLimits<uint32>::Max(); Index++)
		{
			FString DefaultFolderName = Index == 0
				? BaseName.ToString()
				: FString::Printf(TEXT("%s%u"), *BaseName.ToString(), Index);

			if (IsNameUniqueUnderPath(Hierarchy, DefaultFolderName, ParentFolderStr))
			{
				return *(ParentFolderStr.IsEmpty() ? DefaultFolderName : ParentFolderStr / DefaultFolderName);
			}
		}

		UE_LOG(LogLevelStreamAssistant, Warning, TEXT("Could not find unique name for a folder under the path: %s"), *ParentFolder.ToString());

		return NAME_None;
	}

	void FFolderManager::CreateFolder(UWorld& InWorld, const FName& FolderName)
	{
		if (FolderName != NAME_None)
		{
			FHierarchyInfo& Hierarchy = GetHierarchyFor(InWorld);
			if (!Hierarchy.Folders.Find(FolderName))
			{
				FScopedTransaction Transaction(LOCTEXT("TransactionCreateFolder", "Create Folder"));

				ModifyHierarchy(InWorld);

				FFolderProperties NewFolderProperties;
				NewFolderProperties.bIsExpanded = true;
				Hierarchy.AddFolder(FolderName, NewFolderProperties);

				OnFolderRenamed.Broadcast(NAME_None, FolderName);
			}
		}
	}

	void FFolderManager::DeleteFolder(UWorld& InWorld, const FName& FolderToDelete)
	{
		FScopedTransaction Transaction(LOCTEXT("TransactionDeleteFolder", "Delete Folder"));
		
		ModifyHierarchy(InWorld);

		FName ParentPath = *FPaths::GetPath(FolderToDelete.ToString());
		RenameFolder(InWorld, FolderToDelete, ParentPath); // merge folder content with it's parent.
	}

	void FFolderManager::RenameFolder(UWorld& InWorld, const FName& OldFolderName, const FName& NewFolderName)
	{
		if (OldFolderName == NAME_None || OldFolderName == NewFolderName) return;

		FString CommonPath = OldFolderName.ToString();
		FString ReplacementPath = NewFolderName.IsNone() ? TEXT("") : NewFolderName.ToString();

		FScopedTransaction Transaction(LOCTEXT("TransactionRenameFolder", "Rename Folder"));

		ModifyHierarchy(InWorld);

		FHierarchyInfo& Hierarchy = GetHierarchyFor(InWorld);

		TArray<FName> FolderKeys;
		Hierarchy.Folders.GenerateKeyArray(FolderKeys);
		for (const FName& FolderKey : FolderKeys)
		{
			FString OldPath = FolderKey.ToString();
			FString ReplacedPath;
			if (ReplaceCommonPath(OldPath, CommonPath, ReplacementPath, /*OUT*/ReplacedPath))
			{
				Hierarchy.RenameFolder(*OldPath, *ReplacedPath);
				OnFolderRenamed.Broadcast(*OldPath, *ReplacedPath);
			}
		}
	}

	void FFolderManager::ModifyHierarchy(UWorld& InWorld)
	{
		GetHierarchyManagerFor(InWorld).Modify();
	}

	FFolderManager::FHierarchyInfo& FFolderManager::GetHierarchyFor(UWorld& InWorld)
	{
		return GetHierarchyManagerFor(InWorld).FindOrAddHierarchy(FHierarchyInfoKey(bIsForLevels));
	}

	ULevelStreamAssistant_HierarchyManager& FFolderManager::GetHierarchyManagerFor(UWorld& InWorld)
	{
		if (TWeakObjectPtr<ULevelStreamAssistant_HierarchyManager>* Hierarchy = CachedHierarchies.Find(&InWorld))
		{
			if (Hierarchy && Hierarchy->IsValid()) return **Hierarchy;
		}

		ULevelStreamAssistant_HierarchyManager& HierarchyManager = ULevelStreamAssistant_HierarchyManager::GetOrCreateHierarchyManager(InWorld);
		CachedHierarchies.Add(&InWorld, &HierarchyManager);
		return HierarchyManager;
	}

	bool FFolderManager::ReplaceCommonPath(const FString& InPath, const FString& CommonPath, const FString& ReplacementPath, FString& OutPath)
	{
		if (PathStartsWith(InPath, CommonPath))
		{
			const FString& RightChop = InPath.RightChop(CommonPath.Len());
			const FString& LeftChop = ReplacementPath;

			OutPath = !LeftChop.IsEmpty() && !RightChop.IsEmpty() 
				? LeftChop / RightChop
				: LeftChop.IsEmpty() ? RightChop : LeftChop;
			
			return true;
		}
		return false;
	}

	bool FFolderManager::PathStartsWith(const FString& InPath, const FString& InPathToMatch)
	{
		FString Path = InPath;
		Path.RemoveFromStart(InPathToMatch, ESearchCase::CaseSensitive);
		return Path.IsEmpty() || Path[0] == TEXT('/') || Path[0] == TEXT('\\');
	}

	bool FFolderManager::IsNameUniqueUnderPath(FHierarchyInfo& Hierarchy, const FString& InFolderName, const FString& InParentPath) const
	{
		TArray<FName> FolderPaths;
		Hierarchy.Folders.GenerateKeyArray(FolderPaths);
		for (const FName& FolderPath : FolderPaths)
		{
			FString ChildPath = GetPathDirectlyUnderParent(FolderPath.ToString(), InParentPath);

			if (!ChildPath.IsEmpty())
			{
				FString FolderName = FPaths::GetPathLeaf(ChildPath);
				if (FolderName.Compare(InFolderName, ESearchCase::CaseSensitive) == 0)
				{
					return false;
				}
			}
		}
		return true;
	}

	FString FFolderManager::GetPathDirectlyUnderParent(const FString& InPath, const FString& InParent)
	{
		if (InPath.IsEmpty()) return TEXT("");

		FString ParentOfPath = FPaths::GetPath(InPath);

		if (FPaths::IsSamePath(ParentOfPath, InParent))
		{
			return InPath;
		}
		else
		{
			return GetPathDirectlyUnderParent(ParentOfPath, InParent);
		}
	}
}

#undef LOCTEXT_NAMESPACE
