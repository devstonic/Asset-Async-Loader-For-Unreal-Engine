#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "AssetAsyncLoader.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnObjectLoaded, UObject*, loadedObject);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnClassLoaded, UClass*, loadedClass);
DECLARE_DYNAMIC_DELEGATE(FOnLoadFailed);
DECLARE_DYNAMIC_DELEGATE(FOnLoadValid);

UCLASS()
class MYPROJECT_API UAssetAsyncLoader : public UObject
{
    GENERATED_BODY()

public:

    static const int32 DEFAULT_PRIORITY;

    UFUNCTION(BlueprintCallable, Category = "Async Asset Loading")
    static void LoadObjectAsync(const TSoftObjectPtr<UObject> ObjectToLoad,
        const FOnObjectLoaded& OnLoaded,
        int32 Priority = 1);

    UFUNCTION(BlueprintCallable, Category = "Async Asset Loading")
    static void LoadObjectAsyncSimple(const TSoftObjectPtr<UObject> ObjectToLoad,
        const FOnObjectLoaded& OnLoaded);

    UFUNCTION(BlueprintCallable, Category = "Async Asset Loading")
    static void LoadClassAsync(const TSoftClassPtr<UObject> ClassToLoad,
        const FOnClassLoaded& OnLoaded,
        int32 Priority = 1);

    UFUNCTION(BlueprintCallable, Category = "Async Asset Loading")
    static void LoadClassAsyncSimple(const TSoftClassPtr<UObject> ClassToLoad,
        const FOnClassLoaded& OnLoaded);

    UFUNCTION(BlueprintCallable, Category = "Async Asset Loading")
    static void LoadObjectsAsync(const TArray<TSoftObjectPtr<UObject>>& ObjectsToLoad,
        const FOnObjectLoaded& OnLoaded,
        int32 Priority = 1);

    UFUNCTION(BlueprintCallable, Category = "Async Asset Loading")
    static void LoadObjectsAsyncSimple(const TArray<TSoftObjectPtr<UObject>>& ObjectsToLoad,
        const FOnObjectLoaded& OnLoaded);

    UFUNCTION(BlueprintCallable, Category = "Async Asset Loading")
    static void LoadClassesAsync(const TArray<TSoftClassPtr<UObject>>& ClassesToLoad,
        const FOnClassLoaded& OnLoaded,
        int32 Priority = 1);

    UFUNCTION(BlueprintCallable, Category = "Async Asset Loading")
    static void LoadClassesAsyncSimple(const TArray<TSoftClassPtr<UObject>>& ClassesToLoad,
        const FOnClassLoaded& OnLoaded);

    UFUNCTION(BlueprintCallable, Category = "Async Asset Loading", meta = (AutoCreateRefTerm = "OnValid,OnPending,OnFailed"))
    static void PrepareSoftObject(
        const TSoftObjectPtr<UObject> SoftObject,
        const FOnLoadValid& OnValid,
        const FOnObjectLoaded& OnPending,
        const FOnLoadFailed& OnFailed);

    UFUNCTION(BlueprintCallable, Category = "Async Asset Loading", meta = (AutoCreateRefTerm = "OnValid,OnPending,OnFailed"))
    static void PrepareSoftClass(
        const TSoftClassPtr<UObject> SoftClass,
        const FOnLoadValid& OnValid,
        const FOnClassLoaded& OnPending,
        const FOnLoadFailed& OnFailed);

private:

    static void HandleObjectLoaded(TSoftObjectPtr<UObject> TargetObject, FOnObjectLoaded OnLoaded);
    static void HandleClassLoaded(TSoftClassPtr<UObject> TargetClass, FOnClassLoaded OnLoaded);
};