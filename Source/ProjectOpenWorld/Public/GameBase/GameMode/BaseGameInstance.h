#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameFramework/SaveGame.h"
#include "BaseGameInstance.generated.h"

class AActor;

UCLASS()
class PROJECTOPENWORLD_API UPalSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY() FString LevelName;                  // 어느 월드의 스냅샷인지
	UPROPERTY() TArray<uint8> PlayerByteData;       // 플레이어(스탯/인벤토리)
	//UPROPERTY() TArray<FPalActorSaveData> SavedActors; // 동적 액터(건축물/PalBox 등)
};

UCLASS()
class PROJECTOPENWORLD_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TSoftObjectPtr<UWorld> TargetLevel{};

	UPROPERTY(EditDefaultsonly, Category = "Level")
	TSoftObjectPtr<UWorld> LoadingLevel{};
	int Count{};
public:
	UBaseGameInstance();
	UFUNCTION(BlueprintCallable)
	static void LoadLevelStatic(UObject* pWorldContext);
	UFUNCTION(BlueprintCallable)
	static void OpenLevelStatic(UObject* pWorldContext, TSoftObjectPtr<UWorld> newLevel);
	UFUNCTION(BlueprintCallable)
	static void SetTargetLevel(TSoftObjectPtr<UWorld> newLevel);
	UFUNCTION(BlueprintCallable)
	static void PalSaveTest(UObject* pWorldContext, const FString SlotName);
	UFUNCTION(BlueprintCallable)
	static bool LoadFromSlot(UObject* pWorldContext, const FString& SlotName);
private:
	static TObjectPtr<UBaseGameInstance> Instance;
	int32 UserIndex = 0;
	UFUNCTION()
	void OpenLoadedLevel();
	UFUNCTION()
	void ProgressFunction();

	void SerializeActor(AActor* Actor, TArray<uint8>& OutBytes);
	void DeserializeActor(AActor* Actor, const TArray<uint8>& InBytes);
};