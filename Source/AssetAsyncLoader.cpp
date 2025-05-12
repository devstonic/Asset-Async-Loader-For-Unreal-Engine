#include "AssetAsyncLoader.h"
#include "AssetRegistry/AssetRegistryModule.h"

const int32 UAssetAsyncLoader::DEFAULT_PRIORITY = 100;

void UAssetAsyncLoader::LoadObjectAsync(const TSoftObjectPtr<UObject> ObjectToLoad,
	const FOnObjectLoaded& OnLoaded,
	int32 Priority)
{
	if (ObjectToLoad.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to load null object"));
		return;
	}

	if (ObjectToLoad.IsPending())
	{
		FOnObjectLoaded DelegateCopy = OnLoaded;
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
			ObjectToLoad.ToSoftObjectPath(),
			FStreamableDelegate::CreateStatic(&HandleObjectLoaded, ObjectToLoad, DelegateCopy),
			Priority
		);
	}
	else if (ObjectToLoad.IsValid())
	{
		OnLoaded.ExecuteIfBound(ObjectToLoad.Get());
	}
}

void UAssetAsyncLoader::LoadObjectAsyncSimple(const TSoftObjectPtr<UObject> ObjectToLoad,
	const FOnObjectLoaded& OnLoaded)
{
	LoadObjectAsync(ObjectToLoad, OnLoaded, DEFAULT_PRIORITY);
}

void UAssetAsyncLoader::LoadClassAsync(const TSoftClassPtr<UObject> ClassToLoad,
	const FOnClassLoaded& OnLoaded,
	int32 Priority)
{
	if (ClassToLoad.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to load null class"));
		return;
	}

	if (ClassToLoad.IsPending())
	{
		FOnClassLoaded DelegateCopy = OnLoaded;
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
			ClassToLoad.ToSoftObjectPath(),
			FStreamableDelegate::CreateStatic(&HandleClassLoaded, ClassToLoad, DelegateCopy),
			Priority
		);
	}
	else if (ClassToLoad.IsValid())
	{
		OnLoaded.ExecuteIfBound(ClassToLoad.Get());
	}
}

void UAssetAsyncLoader::LoadClassAsyncSimple(const TSoftClassPtr<UObject> ClassToLoad,
	const FOnClassLoaded& OnLoaded)
{
	LoadClassAsync(ClassToLoad, OnLoaded, DEFAULT_PRIORITY);
}

void UAssetAsyncLoader::LoadObjectsAsync(const TArray<TSoftObjectPtr<UObject>>& ObjectsToLoad,
	const FOnObjectLoaded& OnLoaded,
	int32 Priority)
{
	TArray<FSoftObjectPath> PathsToLoad;

	for (const TSoftObjectPtr<UObject>& Object : ObjectsToLoad)
	{
		if (Object.IsPending())
		{
			PathsToLoad.Add(Object.ToSoftObjectPath());
		}
		else if (Object.IsValid())
		{
			OnLoaded.ExecuteIfBound(Object.Get());
		}
	}

	if (PathsToLoad.Num() > 0)
	{
		FOnObjectLoaded DelegateCopy = OnLoaded;
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
			PathsToLoad,
			FStreamableDelegate::CreateLambda([DelegateCopy, ObjectsToLoad]()
				{
					for (const TSoftObjectPtr<UObject>& Object : ObjectsToLoad)
					{
						if (Object.IsValid())
						{
							DelegateCopy.ExecuteIfBound(Object.Get());
						}
					}
				}),
			Priority
		);
	}
}

void UAssetAsyncLoader::LoadObjectsAsyncSimple(const TArray<TSoftObjectPtr<UObject>>& ObjectsToLoad,
	const FOnObjectLoaded& OnLoaded)
{
	LoadObjectsAsync(ObjectsToLoad, OnLoaded, DEFAULT_PRIORITY);
}

void UAssetAsyncLoader::LoadClassesAsync(const TArray<TSoftClassPtr<UObject>>& ClassesToLoad,
	const FOnClassLoaded& OnLoaded,
	int32 Priority)
{
	TArray<FSoftObjectPath> PathsToLoad;

	for (const TSoftClassPtr<UObject>& Class : ClassesToLoad)
	{
		if (Class.IsPending())
		{
			PathsToLoad.Add(Class.ToSoftObjectPath());
		}
		else if (Class.IsValid())
		{
			OnLoaded.ExecuteIfBound(Class.Get());
		}
	}

	if (PathsToLoad.Num() > 0)
	{
		FOnClassLoaded DelegateCopy = OnLoaded;
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
			PathsToLoad,
			FStreamableDelegate::CreateLambda([DelegateCopy, ClassesToLoad]()
				{
					for (const TSoftClassPtr<UObject>& Class : ClassesToLoad)
					{
						if (Class.IsValid())
						{
							DelegateCopy.ExecuteIfBound(Class.Get());
						}
					}
				}),
			Priority
		);
	}
}

void UAssetAsyncLoader::LoadClassesAsyncSimple(const TArray<TSoftClassPtr<UObject>>& ClassesToLoad,
	const FOnClassLoaded& OnLoaded)
{
	LoadClassesAsync(ClassesToLoad, OnLoaded, DEFAULT_PRIORITY);
}

void UAssetAsyncLoader::HandleObjectLoaded(TSoftObjectPtr<UObject> TargetObject, FOnObjectLoaded OnLoaded)
{
	if (TargetObject.IsValid())
	{
		OnLoaded.ExecuteIfBound(TargetObject.Get());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load object: %s"), *TargetObject.ToString());
	}
}

void UAssetAsyncLoader::HandleClassLoaded(TSoftClassPtr<UObject> TargetClass, FOnClassLoaded OnLoaded)
{
	if (TargetClass.IsValid())
	{
		OnLoaded.ExecuteIfBound(TargetClass.Get());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load class: %s"), *TargetClass.ToString());
	}
}

void UAssetAsyncLoader::PrepareSoftObject(
	const TSoftObjectPtr<UObject> SoftObject,
	const FOnLoadValid& OnValid,
	const FOnObjectLoaded& OnPending,
	const FOnLoadFailed& OnFailed)
{
	if (SoftObject.IsNull())
	{
		OnFailed.ExecuteIfBound();
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "AsyncLoader - Soft object is NULL");
		return;
	}

	if (SoftObject.IsValid())
	{
		OnValid.ExecuteIfBound();
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, "AsyncLoader - Soft object is Valid");
		return;
	}

	FSoftObjectPath Path = SoftObject.ToSoftObjectPath();

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, Path.ToString());

	FOnObjectLoaded DelegateCopy = OnPending;
	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		Path,
		FStreamableDelegate::CreateLambda([DelegateCopy, SoftObject]()
			{
				if (SoftObject.IsValid())
				{
					DelegateCopy.ExecuteIfBound(SoftObject.Get());
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, "AsyncLoader - Soft object request to load");
				}
			}),
		DEFAULT_PRIORITY
	);
}

void UAssetAsyncLoader::PrepareSoftClass(
	const TSoftClassPtr<UObject> SoftClass,
	const FOnLoadValid& OnValid,
	const FOnClassLoaded& OnPending,
	const FOnLoadFailed& OnFailed)
{
	if (SoftClass.IsNull())
	{
		OnFailed.ExecuteIfBound();
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "AsyncLoader - Soft class is NULL");
		return;
	}

	if (SoftClass.IsValid())
	{
		OnValid.ExecuteIfBound();
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, "AsyncLoader - Soft class is Valid");
		return;
	}

	FSoftObjectPath Path = SoftClass.ToSoftObjectPath();

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, Path.ToString());

	FOnClassLoaded DelegateCopy = OnPending;
	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		Path,
		FStreamableDelegate::CreateLambda([DelegateCopy, SoftClass]()
			{
				if (SoftClass.IsValid())
				{
					DelegateCopy.ExecuteIfBound(SoftClass.Get());
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, "AsyncLoader - Soft class request to load");
				}
			}),
		DEFAULT_PRIORITY
	);
}