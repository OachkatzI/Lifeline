// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#include "Assistant/Folders/HierarchyManagerObject.h"
#include "Assistant/Folders/FolderManager.h"
#include "Serialization/JsonSerializer.h"
#include <LevelStreamAssistantLog.h>
#include <Editor.h>

const FGuid FLevelStreamAssistant_HierarchyManager_CustomVersion::Key(0xD87062CC, 0x44CE4673, 0x8EBFD9CE, 0x9BF70F26);
static FCustomVersionRegistration LevelStreamAssistant_HierarchyManager_CustomVersionRegistration(
	FLevelStreamAssistant_HierarchyManager_CustomVersion::Key,
	FLevelStreamAssistant_HierarchyManager_CustomVersion::LatestVersion,
	TEXT("LevelStreamAssistant_HierarchyManager_Version"));

void FLevelStreamAssistant_HierarchyInfo::SetFolderPathFor(const UObject& Object, FName NewPath)
{
	if (NewPath == NAME_None)
	{
		ObjectToFolderMap.Remove(&Object);
	}
	else
	{
		ObjectToFolderMap.Add(&Object, NewPath);
	}
}

FName FLevelStreamAssistant_HierarchyInfo::GetFolderPathOf(const UObject& Object) const
{
	if (const FName* FolderPath = ObjectToFolderMap.Find(&Object))
	{
		return *FolderPath;
	}
	return NAME_None;
}

void FLevelStreamAssistant_HierarchyInfo::AddFolder(FName InFolderName, FLevelStreamAssistant_FolderProperties& InFolderProperties)
{
	if (InFolderName != NAME_None)
	{
		if (!Folders.Contains(InFolderName))
		{
			Folders.Add(InFolderName, InFolderProperties);
		}
	}
}

void FLevelStreamAssistant_HierarchyInfo::RenameFolder(FName OldName, FName NewName)
{
	FLevelStreamAssistant_FolderProperties FolderProperties;

	if (Folders.RemoveAndCopyValue(OldName, FolderProperties))
	{
		AddFolder(NewName, FolderProperties);

		TArray<TWeakObjectPtr<const UObject>> ObjectKeys;
		ObjectToFolderMap.GenerateKeyArray(ObjectKeys);
		for (const TWeakObjectPtr<const UObject>& ObjectKey : ObjectKeys)
		{
			FName* ObjectFolderPath = ObjectToFolderMap.Find(ObjectKey);

			if (ensure(ObjectFolderPath) && ObjectKey.IsValid() && ObjectFolderPath->IsEqual(OldName, ENameCase::CaseSensitive))
			{
				SetFolderPathFor(*ObjectKey, NewName);
			}
		}
	}
}

void ULevelStreamAssistant_HierarchyManager::Initialize()
{
	if (bInitialized) return;
	bInitialized = true;

	if (GEditor) GEditor->OnObjectsReplaced().AddUObject(this, &ULevelStreamAssistant_HierarchyManager::HandleObjectsReplaced);

	FEditorDelegates::PreSaveWorld.AddUObject(this, &ULevelStreamAssistant_HierarchyManager::OnPreSaveWorld);
}

ULevelStreamAssistant_HierarchyManager::~ULevelStreamAssistant_HierarchyManager()
{
	if (GEditor) GEditor->OnObjectsReplaced().RemoveAll(this);

	FEditorDelegates::PreSaveWorld.RemoveAll(this);
}

FLevelStreamAssistant_HierarchyInfo& ULevelStreamAssistant_HierarchyManager::FindOrAddHierarchy(const FLevelStreamAssistant_HierarchyInfoKey& Key)
{
	return Hierarchies.FindOrAdd(Key);
}

ULevelStreamAssistant_HierarchyManager& ULevelStreamAssistant_HierarchyManager::GetOrCreateHierarchyManager(UWorld& InWorld)
{
	for (UObject* Object : InWorld.ExtraReferencedObjects)
	{
		if (ULevelStreamAssistant_HierarchyManager* HierarchyManager = Cast<ULevelStreamAssistant_HierarchyManager>(Object))
		{
			HierarchyManager->Initialize();
			return *HierarchyManager;
		}
	}

	ULevelStreamAssistant_HierarchyManager* NewHierarchyManager = NewObject<ULevelStreamAssistant_HierarchyManager>(&InWorld, NAME_None, RF_Transactional);
	
	NewHierarchyManager->Initialize();
	NewHierarchyManager->TryRecoverFromJsonBackup();

	InWorld.ExtraReferencedObjects.Add(NewHierarchyManager);

	return *NewHierarchyManager;
}

void ULevelStreamAssistant_HierarchyManager::Serialize(FArchive& Ar)
{
	Ar.UsingCustomVersion(FLevelStreamAssistant_HierarchyManager_CustomVersion::Key);

	int32 ArchiveVersion = Ar.CustomVer(FLevelStreamAssistant_HierarchyManager_CustomVersion::Key);
	bool bArIsLatestVersion = ArchiveVersion == FLevelStreamAssistant_HierarchyManager_CustomVersion::LatestVersion;
	if (!ensure(bArIsLatestVersion || (!bArIsLatestVersion && Ar.IsLoading()))) return;

	if (ArchiveVersion < FLevelStreamAssistant_HierarchyManager_CustomVersion::ReplaceSoftPtrWithWeakPtr)
	{
		TMap<FLevelStreamAssistant_HierarchyInfoKey, FLevelStreamAssistant_HierarchyInfo_DEPRECATED> Old_Hierarchies;
		Ar << Old_Hierarchies;

		Hierarchies.Empty(Old_Hierarchies.Num());
		for (TPair<FLevelStreamAssistant_HierarchyInfoKey, FLevelStreamAssistant_HierarchyInfo_DEPRECATED> Old_HierarchyKeyValuePair : Old_Hierarchies)
		{
			FLevelStreamAssistant_HierarchyInfo New_HierarchyInfo;

			New_HierarchyInfo.Folders = Old_HierarchyKeyValuePair.Value.Folders;
			New_HierarchyInfo.ObjectToFolderMap.Empty(Old_HierarchyKeyValuePair.Value.ObjectToFolderMap.Num());

			for (TPair<TSoftObjectPtr<const UObject>, FName> Old_ObjectFolderPair : Old_HierarchyKeyValuePair.Value.ObjectToFolderMap)
			{
				New_HierarchyInfo.ObjectToFolderMap.Add(Old_ObjectFolderPair.Key.LoadSynchronous(), Old_ObjectFolderPair.Value);
			}

			Hierarchies.Add(Old_HierarchyKeyValuePair.Key, New_HierarchyInfo);
		}
	}
	else if (ArchiveVersion == FLevelStreamAssistant_HierarchyManager_CustomVersion::LatestVersion)
	{
		Ar << Hierarchies;
	}
	else
	{
		UE_LOG(LogLevelStreamAssistant, Error, TEXT("Folders were saved using a higher plugin version and can not be loaded with the current version of the plugin."));
		TryRecoverFromJsonBackup();
	}
}

void ULevelStreamAssistant_HierarchyManager::HandleObjectsReplaced(const TMap<UObject*, UObject*>& ReplacementMap)
{
	for (const TPair<UObject*, UObject*>& ReplacementKeyValue : ReplacementMap)
	{
		UObject* OldObject = ReplacementKeyValue.Key;
		UObject* NewObject = ReplacementKeyValue.Value;

		for (TPair<FLevelStreamAssistant_HierarchyInfoKey, FLevelStreamAssistant_HierarchyInfo>& HierarchyKeyValue : Hierarchies)
		{
			FLevelStreamAssistant_HierarchyInfo& Hierarchy = HierarchyKeyValue.Value;

			FName ObjectFolderPath;
			if (Hierarchy.ObjectToFolderMap.RemoveAndCopyValue(OldObject, ObjectFolderPath))
			{
				Hierarchy.ObjectToFolderMap.Add(NewObject, ObjectFolderPath);
			}
		}
	}
}

void ULevelStreamAssistant_HierarchyManager::OnPreSaveWorld(uint32 InSaveFlags, UWorld* InWorld)
{
	if (InWorld == GetOuter())
	{
		CleanUp();

		if (Hierarchies.Num() <= 0 && InWorld)
		{
			InWorld->ExtraReferencedObjects.Remove(this);
			DeleteJsonBackup();
		}
		else
		{
			WriteJsonBackup();
		}
	}
}

void ULevelStreamAssistant_HierarchyManager::CleanUp()
{
	TArray<FLevelStreamAssistant_HierarchyInfoKey> HierarchyKeys;
	Hierarchies.GenerateKeyArray(HierarchyKeys);
	for (const FLevelStreamAssistant_HierarchyInfoKey& HierarchyKey : HierarchyKeys)
	{
		FLevelStreamAssistant_HierarchyInfo* Hierarchy = Hierarchies.Find(HierarchyKey);
		if (ensure(Hierarchy))
		{
			// cleanup missing objects.
			TArray<TWeakObjectPtr<const UObject>> ObjectKeys;
			Hierarchy->ObjectToFolderMap.GenerateKeyArray(ObjectKeys);
			for (const TWeakObjectPtr<const UObject>& ObjectKey : ObjectKeys)
			{
				if (!ObjectKey.IsValid())
				{
					Hierarchy->ObjectToFolderMap.Remove(ObjectKey);
				}
			}

			if (Hierarchy->ObjectToFolderMap.Num() <= 0 && Hierarchy->Folders.Num() <= 0)
			{
				Hierarchies.Remove(HierarchyKey);
			}
		}
	}
}

void ULevelStreamAssistant_HierarchyManager::WriteJsonBackup()
{
	const FString Filename = GetJsonPathForBackup();
	TUniquePtr<FArchive> Ar(IFileManager::Get().CreateFileWriter(*Filename));
	if (Ar)
	{
		TSharedRef<FJsonObject> RootObject = MakeShareable(new FJsonObject);
		{
			TArray<TSharedPtr<FJsonValue>> JsonHierarchies;
			JsonHierarchies.Reserve(Hierarchies.Num());
			for (const TPair<FLevelStreamAssistant_HierarchyInfoKey, FLevelStreamAssistant_HierarchyInfo>& Hierarchy : Hierarchies)
			{
				JsonHierarchies.Add(MakeShareable(new FJsonValueObject(SerializeHierarchy(Hierarchy))));
			}

			RootObject->SetArrayField(TEXT("Hierarchies"), JsonHierarchies);
		}

		TSharedRef<TJsonWriter<TCHAR>> Writer = TJsonWriterFactory<TCHAR>::Create(Ar.Get());
		FJsonSerializer::Serialize(RootObject, Writer);
		Ar->Close();
	}
}

void ULevelStreamAssistant_HierarchyManager::DeleteJsonBackup()
{
	IFileManager::Get().Delete(*GetJsonPathForBackup());
}

void ULevelStreamAssistant_HierarchyManager::TryRecoverFromJsonBackup()
{
	const FString Filename = GetJsonPathForBackup();

	Hierarchies.Empty();

	TUniquePtr<FArchive> Ar(IFileManager::Get().CreateFileReader(*Filename));
	if (Ar)
	{
		TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Ar.Get());

		TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);
		if (FJsonSerializer::Deserialize(Reader, /*OUT*/RootObject))
		{
			TArray<TSharedPtr<FJsonValue>> JsonHierarchies = RootObject->GetArrayField(TEXT("Hierarchies"));

			for (TSharedPtr<FJsonValue> JsonHierarchy : JsonHierarchies)
			{
				if (JsonHierarchy.IsValid())
				{
					Hierarchies.Add(DeserializeHierarchy(JsonHierarchy->AsObject()));
				}
			}

			CleanUp();

			if (Hierarchies.Num() > 0)
			{
				UE_LOG(LogLevelStreamAssistant, Log, TEXT("Some folders were recovered from json backup file %s."), *Filename);
			}
		}

		Ar->Close();
	}
}

FString ULevelStreamAssistant_HierarchyManager::GetJsonPathForBackup() const
{
	const FString PathName = GetOutermost()->GetPathName();
	const uint32 PathNameCrc = FCrc::MemCrc32(*PathName, sizeof(TCHAR) * PathName.Len());
	return FPaths::Combine(*FPaths::ProjectSavedDir(), TEXT("Config"), TEXT("LevelStreamAssistant"), *FString::Printf(TEXT("hierarchy.%u.json"), PathNameCrc));
}

TSharedRef<FJsonObject> ULevelStreamAssistant_HierarchyManager::SerializeHierarchy(const TPair<FLevelStreamAssistant_HierarchyInfoKey, FLevelStreamAssistant_HierarchyInfo>& Hierarchy)
{
	TSharedRef<FJsonObject> JsonHierarchy = MakeShareable(new FJsonObject);
	JsonHierarchy->SetObjectField(TEXT("Key"), SerializeHierarchyInfoKey(Hierarchy.Key));
	JsonHierarchy->SetObjectField(TEXT("Value"), SerializeHierarchyInfo(Hierarchy.Value));
	return JsonHierarchy;
}

TPair<FLevelStreamAssistant_HierarchyInfoKey, FLevelStreamAssistant_HierarchyInfo> ULevelStreamAssistant_HierarchyManager::DeserializeHierarchy(TSharedPtr<FJsonObject> JsonHierarchy)
{
	TPair<FLevelStreamAssistant_HierarchyInfoKey, FLevelStreamAssistant_HierarchyInfo> Hierarchy;
	if (JsonHierarchy.IsValid())
	{
		Hierarchy.Key = DeserializeHierarchyInfoKey(JsonHierarchy->GetObjectField(TEXT("Key")));
		Hierarchy.Value = DeserializeHierarchyInfo(JsonHierarchy->GetObjectField(TEXT("Value")));
	}
	return Hierarchy;
}

TSharedRef<FJsonObject> ULevelStreamAssistant_HierarchyManager::SerializeHierarchyInfoKey(const FLevelStreamAssistant_HierarchyInfoKey& HierarchyInfo)
{
	TSharedRef<FJsonObject> JsonHierarchyInfo = MakeShareable(new FJsonObject);
	JsonHierarchyInfo->SetBoolField(TEXT("bIsForLevels"), HierarchyInfo.bIsForLevels);
	JsonHierarchyInfo->SetStringField(TEXT("HierarchyName"), HierarchyInfo.HierarchyName.IsNone() ? TEXT("") : HierarchyInfo.HierarchyName.ToString());
	return JsonHierarchyInfo;
}

FLevelStreamAssistant_HierarchyInfoKey ULevelStreamAssistant_HierarchyManager::DeserializeHierarchyInfoKey(TSharedPtr<FJsonObject> JsonHierarchyInfo)
{
	FLevelStreamAssistant_HierarchyInfoKey HierarchyKey;
	if (JsonHierarchyInfo.IsValid())
	{
		bool bIsForLevels = false;
		HierarchyKey.bIsForLevels = JsonHierarchyInfo->TryGetBoolField(TEXT("bIsForLevels"), bIsForLevels) && bIsForLevels;

		FString HierarchyName = TEXT("");
		if (JsonHierarchyInfo->TryGetStringField(TEXT("HierarchyName"), HierarchyName))
		{
			HierarchyKey.HierarchyName = *HierarchyName;
		}
	}
	return HierarchyKey;
}

TSharedRef<FJsonObject> ULevelStreamAssistant_HierarchyManager::SerializeHierarchyInfo(const FLevelStreamAssistant_HierarchyInfo& HierarchyInfo)
{
	TSharedRef<FJsonObject> JsonHierarchyInfo = MakeShareable(new FJsonObject);
	JsonHierarchyInfo->SetObjectField(TEXT("Folders"), SerializeFoldersMap(HierarchyInfo.Folders));
	JsonHierarchyInfo->SetObjectField(TEXT("ObjectToFolder"), SerializeObjectToFolderMap(HierarchyInfo.ObjectToFolderMap));
	return JsonHierarchyInfo;
}

FLevelStreamAssistant_HierarchyInfo ULevelStreamAssistant_HierarchyManager::DeserializeHierarchyInfo(TSharedPtr<FJsonObject> JsonHierarchyInfo)
{
	FLevelStreamAssistant_HierarchyInfo HierarchyInfo;
	if (JsonHierarchyInfo.IsValid())
	{
		HierarchyInfo.Folders = DeserializeFoldersMap(JsonHierarchyInfo->GetObjectField(TEXT("Folders")));
		HierarchyInfo.ObjectToFolderMap = DeserializeObjectToFolderMap(JsonHierarchyInfo->GetObjectField(TEXT("ObjectToFolder")));
	}
	return HierarchyInfo;
}

TSharedRef<FJsonObject> ULevelStreamAssistant_HierarchyManager::SerializeObjectToFolderMap(const TMap<TWeakObjectPtr<const UObject>, FName>& ObjectToFolderMap)
{
	TSharedRef<FJsonObject> JsonObjectToFolderMap = MakeShareable(new FJsonObject);
	for (const TPair<TWeakObjectPtr<const UObject>, FName>& KeyValue : ObjectToFolderMap)
	{
		JsonObjectToFolderMap->SetStringField(FSoftObjectPath(KeyValue.Key.Get()).ToString(), KeyValue.Value.ToString());
	}
	return JsonObjectToFolderMap;
}

TMap<TWeakObjectPtr<const UObject>, FName> ULevelStreamAssistant_HierarchyManager::DeserializeObjectToFolderMap(TSharedPtr<FJsonObject> JsonObjectToFolderMap)
{
	TMap<TWeakObjectPtr<const UObject>, FName> ObjectToFolderMap;
	if (JsonObjectToFolderMap.IsValid())
	{
		for (const TPair<FString, TSharedPtr<FJsonValue>>& KeyValue : JsonObjectToFolderMap->Values)
		{
			if (KeyValue.Value.IsValid())
			{
				FString FolderPath;
				if (KeyValue.Value->TryGetString(FolderPath))
				{
					ObjectToFolderMap.Add(TSoftObjectPtr<const UObject>(FSoftObjectPath(KeyValue.Key)).LoadSynchronous(), *FolderPath);
				}
			}
		}
	}
	return ObjectToFolderMap;
}

TSharedRef<FJsonObject> ULevelStreamAssistant_HierarchyManager::SerializeFoldersMap(const TMap<FName, FLevelStreamAssistant_FolderProperties>& FolderMap)
{
	TSharedRef<FJsonObject> JsonFolderMap = MakeShareable(new FJsonObject);
	for (const TPair<FName, FLevelStreamAssistant_FolderProperties>& KeyValue : FolderMap)
	{
		JsonFolderMap->SetObjectField(KeyValue.Key.ToString(), SerializeFolderProperties(KeyValue.Value));
	}
	return JsonFolderMap;
}

TMap<FName, FLevelStreamAssistant_FolderProperties> ULevelStreamAssistant_HierarchyManager::DeserializeFoldersMap(TSharedPtr<FJsonObject> JsonFolderMap)
{
	TMap<FName, FLevelStreamAssistant_FolderProperties> FolderMap;
	if (JsonFolderMap.IsValid())
	{
		for (const TPair<FString, TSharedPtr<FJsonValue>>& KeyValue : JsonFolderMap->Values)
		{
			if (KeyValue.Value.IsValid())
			{
				FolderMap.Add(*KeyValue.Key, DeserializeFolderProperties(KeyValue.Value->AsObject()));
			}
		}
	}
	return FolderMap;
}

TSharedRef<FJsonObject> ULevelStreamAssistant_HierarchyManager::SerializeFolderProperties(const FLevelStreamAssistant_FolderProperties& FolderProperties)
{
	TSharedRef<FJsonObject> JsonFolderProperties = MakeShareable(new FJsonObject);
	JsonFolderProperties->SetArrayField(TEXT("Color"), SerializeFolderColor(FolderProperties.Color));
	JsonFolderProperties->SetField(TEXT("bIsExpanded"), SerializeFolderExpanded(FolderProperties.bIsExpanded));
	return JsonFolderProperties;
}

FLevelStreamAssistant_FolderProperties ULevelStreamAssistant_HierarchyManager::DeserializeFolderProperties(TSharedPtr<FJsonObject> JsonFolderProperties)
{
	FLevelStreamAssistant_FolderProperties FolderProperties;
	if (JsonFolderProperties.IsValid())
	{
		FolderProperties.Color = DeserializeFolderColor(JsonFolderProperties->GetArrayField(TEXT("Color")));
		FolderProperties.bIsExpanded = DeserializeFolderExpanded(JsonFolderProperties->GetField<EJson::Boolean>(TEXT("bIsExpanded")));
	}
	return FolderProperties;
}

TSharedRef<FJsonValue> ULevelStreamAssistant_HierarchyManager::SerializeFolderExpanded(bool bIsExpanded)
{
	return MakeShareable(new FJsonValueBoolean(bIsExpanded));
}

bool ULevelStreamAssistant_HierarchyManager::DeserializeFolderExpanded(TSharedPtr<FJsonValue> JsonFolderExpanded)
{
	bool bIsExpanded = false;
	return JsonFolderExpanded->TryGetBool(bIsExpanded) && bIsExpanded;
}

TArray<TSharedPtr<FJsonValue>> ULevelStreamAssistant_HierarchyManager::SerializeFolderColor(const FColor& Color)
{
	TSharedRef<FJsonValue> JsonR = MakeShareable(new FJsonValueNumberString(FString::FromInt(Color.R)));
	TSharedRef<FJsonValue> JsonG = MakeShareable(new FJsonValueNumberString(FString::FromInt(Color.G)));
	TSharedRef<FJsonValue> JsonB = MakeShareable(new FJsonValueNumberString(FString::FromInt(Color.B)));
	return { JsonR, JsonG, JsonB };
}

FColor ULevelStreamAssistant_HierarchyManager::DeserializeFolderColor(const TArray<TSharedPtr<FJsonValue>>& JsonFolderColor)
{
	FColor FolderColor = FColor::White;
	if (JsonFolderColor.Num() == 3)
	{
		FolderColor.R = FMath::Clamp(FCString::Atoi(*JsonFolderColor[0]->AsString()), 0, 255);
		FolderColor.G = FMath::Clamp(FCString::Atoi(*JsonFolderColor[1]->AsString()), 0, 255);
		FolderColor.B = FMath::Clamp(FCString::Atoi(*JsonFolderColor[2]->AsString()), 0, 255);
	}
	return FolderColor;
}
