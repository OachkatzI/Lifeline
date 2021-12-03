// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <UObject/Object.h>
#include <Assistant/Folders/FolderManager.h>
#include "HierarchyManagerObject.generated.h"

class FJsonObject;
class FJsonValue;

// Version used to serialize ULevelStreamAssistant_HierarchyManager
struct FLevelStreamAssistant_HierarchyManager_CustomVersion
{
	static const FGuid Key;
	enum Type
	{
		Initial,
		ReplaceSoftPtrWithWeakPtr,					// using TWeakObjectPtr instead of TSoftObjectPtr in ObjectToFolderMap.
		LatestVersion = ReplaceSoftPtrWithWeakPtr	// this should be always the latest value.
	};

	FLevelStreamAssistant_HierarchyManager_CustomVersion() = delete;
};

struct FLevelStreamAssistant_HierarchyInfo_DEPRECATED
{
	TMap<FName, FLevelStreamAssistant_FolderProperties> Folders;
	TMap<TSoftObjectPtr<const UObject>, FName> ObjectToFolderMap;

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, FLevelStreamAssistant_HierarchyInfo_DEPRECATED& HierarchyInfo)
	{
		Ar << HierarchyInfo.Folders;
		Ar << HierarchyInfo.ObjectToFolderMap;
		return Ar;
	}
};

USTRUCT()
struct FLevelStreamAssistant_HierarchyInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TMap<FName, FLevelStreamAssistant_FolderProperties> Folders;
	
	UPROPERTY(Transient)
	TMap<TWeakObjectPtr<const UObject>, FName> ObjectToFolderMap;

	void SetFolderPathFor(const UObject& Object, FName NewPath);
	FName GetFolderPathOf(const UObject& Object) const;

	void AddFolder(FName InFolderName, FLevelStreamAssistant_FolderProperties& InFolderProperties);
	void RenameFolder(FName OldName, FName NewName);

	/** Serializer. */
	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, FLevelStreamAssistant_HierarchyInfo& HierarchyInfo)
	{
		Ar << HierarchyInfo.Folders;
		Ar << HierarchyInfo.ObjectToFolderMap;
		return Ar;
	}
};

USTRUCT()
struct FLevelStreamAssistant_HierarchyInfoKey
{
	GENERATED_BODY()

public:
	FLevelStreamAssistant_HierarchyInfoKey() : bIsForLevels(false) { }
	FLevelStreamAssistant_HierarchyInfoKey(bool bIsForLevels) : bIsForLevels(bIsForLevels) { }

	UPROPERTY(Transient)
	bool bIsForLevels = false;
		
	UPROPERTY(Transient)
	FName HierarchyName = NAME_None;

	friend bool operator==(const FLevelStreamAssistant_HierarchyInfoKey& A, const FLevelStreamAssistant_HierarchyInfoKey& B)
	{
		return A.bIsForLevels == B.bIsForLevels && A.HierarchyName == B.HierarchyName;
	}

	friend uint32 GetTypeHash(const FLevelStreamAssistant_HierarchyInfoKey& ItemID)
	{
		return HashCombine(GetTypeHash(ItemID.bIsForLevels), GetTypeHash(ItemID.HierarchyName));
	}

	/** Serializer. */
	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, FLevelStreamAssistant_HierarchyInfoKey& HierarchyKey)
	{
		Ar << HierarchyKey.bIsForLevels;
		Ar << HierarchyKey.HierarchyName;
		return Ar;
	}
};

struct FAssetRenameData;
class ULevelStreamAssistant_WorldFolders;

UCLASS()
class ULevelStreamAssistant_HierarchyManager : public UObject
{
	GENERATED_BODY()

private:
	typedef ULevelStreamAssistant_WorldFolders UWorldFolders;

public:
	~ULevelStreamAssistant_HierarchyManager();

	FLevelStreamAssistant_HierarchyInfo& FindOrAddHierarchy(const FLevelStreamAssistant_HierarchyInfoKey& Key);

	virtual bool IsEditorOnly() const override final { return true; }

	static ULevelStreamAssistant_HierarchyManager& GetOrCreateHierarchyManager(UWorld& InWorld);

	virtual void Serialize(FArchive& Ar) override;

private:
	void Initialize();
	
	void HandleObjectsReplaced(const TMap<UObject*, UObject*>& ReplacementMap);
	void OnPreSaveWorld(uint32 InSaveFlags, class UWorld* InWorld);
	
	void CleanUp();

	void WriteJsonBackup();
	void DeleteJsonBackup();
	void TryRecoverFromJsonBackup();

	FString GetJsonPathForBackup() const;

	UPROPERTY()
	TMap<FLevelStreamAssistant_HierarchyInfoKey, FLevelStreamAssistant_HierarchyInfo> Hierarchies;

	bool bInitialized = false;

	TSharedRef<FJsonObject> SerializeHierarchy(const TPair<FLevelStreamAssistant_HierarchyInfoKey, FLevelStreamAssistant_HierarchyInfo>& Hierarchy);
	TPair<FLevelStreamAssistant_HierarchyInfoKey, FLevelStreamAssistant_HierarchyInfo> DeserializeHierarchy(TSharedPtr<FJsonObject> JsonHierarchy);

	TSharedRef<FJsonObject> SerializeHierarchyInfoKey(const FLevelStreamAssistant_HierarchyInfoKey& HierarchyInfo);
	FLevelStreamAssistant_HierarchyInfoKey DeserializeHierarchyInfoKey(TSharedPtr<FJsonObject> JsonHierarchyInfo);

	TSharedRef<FJsonObject> SerializeHierarchyInfo(const FLevelStreamAssistant_HierarchyInfo& HierarchyInfo);
	FLevelStreamAssistant_HierarchyInfo DeserializeHierarchyInfo(TSharedPtr<FJsonObject> JsonHierarchyInfo);

	TSharedRef<FJsonObject> SerializeObjectToFolderMap(const TMap<TWeakObjectPtr<const UObject>, FName>& ObjectToFolderMap);
	TMap<TWeakObjectPtr<const UObject>, FName> DeserializeObjectToFolderMap(TSharedPtr<FJsonObject> JsonObjectToFolderMap);

	TSharedRef<FJsonObject> SerializeFoldersMap(const TMap<FName, FLevelStreamAssistant_FolderProperties>& FolderMap);
	TMap<FName, FLevelStreamAssistant_FolderProperties> DeserializeFoldersMap(TSharedPtr<FJsonObject> JsonFolderMap);

	TSharedRef<FJsonObject> SerializeFolderProperties(const FLevelStreamAssistant_FolderProperties& FolderProperties);
	FLevelStreamAssistant_FolderProperties DeserializeFolderProperties(TSharedPtr<FJsonObject> JsonFolderProperties);

	TSharedRef<FJsonValue> SerializeFolderExpanded(bool bIsExpanded);
	bool DeserializeFolderExpanded(TSharedPtr<FJsonValue> JsonFolderExpanded);

	TArray<TSharedPtr<FJsonValue>> SerializeFolderColor(const FColor& Color);
	FColor DeserializeFolderColor(const TArray<TSharedPtr<FJsonValue>>& JsonFolderColor);
};
