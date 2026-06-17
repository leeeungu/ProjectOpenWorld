#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonsterHpLayer.generated.h"

class UCanvasPanel;
class UPalHpWidget;
class UPalHpWidget_Boss;
class APalBaseMonster;

USTRUCT()
struct FHpBarEntry
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UPalHpWidget> Widget = nullptr;   // GC 유지용
};

UCLASS()
class PROJECTOPENWORLD_API UMonsterHpLayer : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr < UPalHpWidget_Boss> BossHpWidget{};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> BarCanvas{};

	UPROPERTY(EditAnywhere, Category = "HP UI")
	TSubclassOf<UPalHpWidget> MonsterBarClass{};

	// 시작 시 미리 만들어 둘 바 개수(풀 프리워밍). 부족하면 런타임에 추가 생성.
	UPROPERTY(EditAnywhere, Category = "HP UI", meta = (ClampMin = "0"))
	int32 InitialPoolSize = 8;

	UPROPERTY(EditAnywhere, Category = "HP UI")
	float NearDistance = 800.f;   // 이 거리 이하면 풀사이즈

	UPROPERTY(EditAnywhere, Category = "HP UI", meta = (ClampMin = "0.05", ClampMax = "1.0"))
	float MinUIScale = 0.4f;

	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, FHpBarEntry> Entries{};
	UPROPERTY()
	TArray<TObjectPtr<UPalHpWidget>> Pool{};
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "HP UI")
	bool bCreateInEdit{};
	TObjectPtr<UPalHpWidget> EditorWidget{};
#endif


	UPalHpWidget* CreateBar();
	UPalHpWidget* AcquireBar();
	void ReleaseBar(UPalHpWidget* Bar);
	void PrewarmPool();

	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	// 일반 몬스터: HP 소켓 기준으로 따라다니는 바

	void RegisterMonster(AActor* Target);
	void UnregisterMonster(AActor* Target);
};
