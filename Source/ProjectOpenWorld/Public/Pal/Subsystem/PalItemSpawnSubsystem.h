#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PalItemSpawnSubsystem.generated.h"

class UStaticMeshComponent;
class AItemActor;

UCLASS()
class PROJECTOPENWORLD_API UPalItemSpawnSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
//    /** 채집물용 — StaticMesh의 AssetUserData 기반 */
//   UFUNCTION(BlueprintCallable, Category = "Pal|Item")
//   static AItemActor* SpawnItemsFromMesh(UWorld* World, FName ItemName, FTransform Origin, AActor* Owner = nullptr);
//
//    /** 캐릭터/몬스터용 — 캐릭터 ID로 CharacterDataSubsystem 조회 */
//   UFUNCTION(BlueprintCallable, Category = "Pal|Item")
//   static int32 SpawnItemsForCharacter(FName CharacterID, const FTransform& Origin, AActor* Causer = nullptr);
//
//    UPROPERTY(EditAnywhere, Category = "Pal|Item")
//    FVector RandomXYOffsetRange = FVector(50.f, 50.f, 0.f);
//
//    UPROPERTY(EditAnywhere, Category = "Pal|Item")
//    float SpawnZOffset = 20.f;
//
//private:
//    /** 드롭 리스트 받아서 확률 굴리고 스폰 — 두 public이 공유 */
//    int32 SpawnFromDropList(const TArray<FPalItemDropData>& Drops, const FTransform& Origin, AActor* Causer);
//
//    /** 단일 아이템 — leaf (실제 클래스 생성 지점) */
//    void SpawnSingleItem(FName ItemID, int32 Count, const FTransform& Location);
//
//    FTransform MakeSpawnTransform(const FTransform& BaseOrigin) const;
};