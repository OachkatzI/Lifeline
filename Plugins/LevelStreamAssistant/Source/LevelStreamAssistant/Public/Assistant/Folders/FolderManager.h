// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <FolderManager.generated.h>

class ULevelStreamAssistant_HierarchyManager;
struct FLevelStreamAssistant_HierarchyInfo;
struct FLevelStreamAssistant_HierarchyInfoKey;

USTRUCT()
struct FLevelStreamAssistant_FolderProperties
{
	GENERATED_BODY()

public:
	/** Serializer. */
	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, FLevelStreamAssistant_FolderProperties& FolderProperties)
	{
		Ar << FolderProperties.Color;
		Ar << FolderProperties.bIsExpanded;
		return Ar;
	}

	UPROPERTY()
	FColor Color = FColor::White;

	UPROPERTY(Transient)
	bool bIsExpanded = true;
};

namespace LevelStreamAssistant
{
	typedef FLevelStreamAssistant_FolderProperties FFolderProperties;

	class FFolderManager
	{
		typedef FLevelStreamAssistant_HierarchyInfo FHierarchyInfo;
		typedef FLevelStreamAssistant_HierarchyInfoKey FHierarchyInfoKey;

	public:
		DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFolderRenamed, const FName& /*OldPath*/, const FName& /*NewPath*/);
		DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnObjectFolderChanged, const UObject*, const FName& /*OldPath*/, const FName& /*NewPath*/);

		FOnFolderRenamed OnFolderRenamed;
		FOnObjectFolderChanged OnObjectFolderChanged;

	private:
		FFolderManager(bool bIsForLevels);
	public:
		~FFolderManager();

		static FFolderManager& GetActorFolderManager();
		static FFolderManager& GetLevelFolderManager();

		static void Initialize();
		static void Shutdown();

		FName GetFolderPathOf(UWorld& InWorld, TWeakObjectPtr<const UObject> InObject);

		void SetFolderPathTo(UWorld& InWorld, TWeakObjectPtr<const UObject> InObject, FName Path);

		FFolderProperties* GetFolderProperties(UWorld& InWorld, const FName& FolderName);

		TArray<FName> GetAllFolders(UWorld& InWorld);

		FName GetUniqueFolderName(UWorld& InWorld, const FName& ParentFolder, const FName& BaseName = TEXT("NewFolder"));

		void CreateFolder(UWorld& InWorld, const FName& FolderName);
		void DeleteFolder(UWorld& InWorld, const FName& FolderToDelete);
		void RenameFolder(UWorld& InWorld, const FName& OldFolderName, const FName& NewFolderName);

		void ModifyHierarchy(UWorld& InWorld);

	private:
		FHierarchyInfo& GetHierarchyFor(UWorld& InWorld);
		ULevelStreamAssistant_HierarchyManager& GetHierarchyManagerFor(UWorld& InWorld);

		static bool ReplaceCommonPath(const FString& InPath, const FString& CommonPath, const FString& ReplacementPath, FString& OutPath);
		static bool PathStartsWith(const FString& InPath, const FString& InPathToMatch);

		bool IsNameUniqueUnderPath(FHierarchyInfo& WorldFolders, const FString& InFolderName, const FString& InPath) const;

		static FString GetPathDirectlyUnderParent(const FString& InPath, const FString& InParent);

		bool bIsForLevels = false;

		TMap<TWeakObjectPtr<const UWorld>, TWeakObjectPtr<ULevelStreamAssistant_HierarchyManager>> CachedHierarchies;

		static FFolderManager* ActorFolderManager;
		static FFolderManager* LevelFolderManager;
	};
}
