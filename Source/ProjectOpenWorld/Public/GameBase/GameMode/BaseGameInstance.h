#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameFramework/SaveGame.h"
#include "BaseGameInstance.generated.h"

class AActor;

// 월드에 배치된 동적 액터(빌딩 등) 1개의 저장 레코드
USTRUCT()
struct FPalActorSaveData
{
	GENERATED_BODY()
public:
	UPROPERTY() FSoftClassPath ActorClass {};   // 재생성할 클래스
	UPROPERTY() FTransform     Transform {};    // 월드 배치 트랜스폼
	UPROPERTY() TArray<uint8>  ByteData{};     // 액터+컴포넌트 SaveGame 스냅샷
};

UCLASS()
class PROJECTOPENWORLD_API UPalSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY() FString LevelName {};                     // 어느 월드의 스냅샷인지
	UPROPERTY() TArray<uint8> PlayerByteData{};          // 플레이어(스탯/인벤토리)
	UPROPERTY() TArray<FPalActorSaveData> SavedActors{}; // 월드 배치 빌딩
};

UCLASS()
class PROJECTOPENWORLD_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TSoftObjectPtr<UWorld> TargetLevel{};

	UPROPERTY(EditDefaultsOnly, Category = "Level")
	TSoftObjectPtr<UWorld> LoadingLevel{};

	// 빌딩 저장/복원이 허용되는 레벨(보스 맵 등은 제외) — GameInstance BP 디폴트에서 설정
	UPROPERTY(EditDefaultsOnly, Category = "Save")
	TArray<TSoftObjectPtr<UWorld>> BuildingLevels{};

	int Count{};

public:
	UBaseGameInstance();

	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable)
	static void LoadLevelStatic(UObject* pWorldContext);
	UFUNCTION(BlueprintCallable)
	static void OpenLevelStatic(UObject* pWorldContext, TSoftObjectPtr<UWorld> newLevel);
	UFUNCTION(BlueprintCallable)
	static void SetTargetLevel(TSoftObjectPtr<UWorld> newLevel);

	// New Game 시작 시 호출 — 기존 슬롯 삭제(이전 플레이 빌딩/플레이어 복원 방지)
	UFUNCTION(BlueprintCallable)
	static void ClearSaves();

	UFUNCTION(BlueprintCallable)
	static void PalSaveTest(UObject* pWorldContext, const FString SlotName);
	UFUNCTION(BlueprintCallable)
	static bool LoadFromSlot(UObject* pWorldContext, const FString& SlotName); // 수동/디버그용

	static void SavePlayer(UObject* pWorldContext);
private:
	static TObjectPtr<UBaseGameInstance> Instance;
	int32 UserIndex = 0;

	// 플레이어는 레벨 간 유지 → 고정 슬롯 / 빌딩은 레벨별 슬롯
	static const FString PlayerSlotName;

	UFUNCTION()
	void OpenLoadedLevel();
	UFUNCTION()
	void ProgressFunction();

	// 맵 이동 직전(현재 월드 살아있을 때) 저장
	UFUNCTION(BlueprintCallable)
	static void SaveWorld();
	void SaveCurrentWorld(UWorld* World);
	// 맵 로드 완료 콜백 → 다음 틱에 복원 예약
	void OnPostLoadMap(UWorld* LoadedWorld);
	// 실제 복원(폰/액터 준비 완료 후)
	void ApplyLoad(UWorld* World, const FString& LevelSlot);
public:
	static void SerializeActor(AActor* Actor, TArray<uint8>& OutBytes);
	static void DeserializeActor(AActor* Actor, const TArray<uint8>& InBytes);
private:
	// 현재 맵이 빌딩 허용 레벨인지
	bool IsBuildingLevel(UWorld* World) const;

	// 레벨 짧은 이름(PIE 프리픽스 제거) / 슬롯 이름
	static FString GetShortMapName(UWorld* World);
	static FString MakeLevelSlotName(UWorld* World);

	FDelegateHandle PostLoadMapHandle{};
};