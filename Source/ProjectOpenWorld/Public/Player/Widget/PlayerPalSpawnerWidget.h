#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerPalSpawnerWidget.generated.h"

class UPalStorageComponent;
class UPalHpWidget_MonsterDefault;
class UImage;

/**
 * 좌측 하단 펠 선택/스폰 위젯.
 * - PalStorageComponent의 보관 펠을 3슬롯 캐러셀로 표시 (좌=이전 / 중앙=선택 / 우=다음)
 * - 슬롯 이미지는 펠 아이콘(PalCharacterDataSubsystem),
 *   중앙 레벨/이름/HP는 WBP_PalMonsterHP(UPalHpWidget_MonsterDefault) 재사용
 * - 입력은 외부(컨트롤러/BP)에서 RotateSelection / GetSelectedPal 호출 (이 위젯은 입력 미처리)
 * - 회전 시 슬라이드/확대 애니메이션은 OnSelectionRotated 훅으로 BP에서 처리
 */
UCLASS()
class PROJECTOPENWORLD_API UPlayerPalSpawnerWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	// === 디자이너에서 동일 이름으로 BindWidget ===
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> LeftSlotImage{};
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CenterSlotImage{};
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> RightSlotImage{};
	// WBP_PalMonsterHP 인스턴스 (중앙 펠의 레벨/이름/HP). 아직 미배치면 BindWidgetOptional로 바꿔도 됨
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPalHpWidget_MonsterDefault> CenterHpWidget{};

	// 데이터 소스
	TWeakObjectPtr<UPalStorageComponent> Storage{};
	// 보관 슬롯에서 null 제외한 압축 목록 (캐러셀 대상)
	int32 SelectedIndex = 0;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// 현재 SelectedIndex 기준으로 좌/중앙/우 슬롯 갱신
	void UpdateVisibleSlots();
	// V1/V2 · creature/monster 공통 펠 이름 조회
	FName ResolvePalName(AActor* Pal) const;
public:
	// 외부에서 스토리지 주입 (미지정 시 NativeConstruct에서 소유 폰으로 자동 탐색)
	UFUNCTION(BlueprintCallable, Category = "PalSpawner")
	void InitWidget(UPalStorageComponent* InStorage);

	// 입력 레이어에서 호출: Direction -1(키 1) / +1(키 3)
	UFUNCTION(BlueprintCallable, Category = "PalSpawner")
	void RotateSelection(int32 Direction);

	int32 GetSelectedPal() const { return SelectedIndex; }

	// 캐러셀 회전 시 BP에서 슬라이드/확대 애니메이션 재생용 훅 (Direction: -1/+1)
	UFUNCTION(BlueprintImplementableEvent, Category = "PalSpawner")
	void OnSelectionRotated(int32 Direction);

protected:
	void SetSlot(UImage* Img, AActor* Pal) const;
	void UpdateCenterInfo(AActor* Center) const;
	UFUNCTION()
	void OnPalStoreChanged(int ChangeIndex, AActor* ChangedPal);
};