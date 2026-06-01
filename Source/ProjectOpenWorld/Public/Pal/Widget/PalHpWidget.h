#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalHpWidget.generated.h"

class UStatComponent;
enum class EStatusType :uint8;

UCLASS(Abstract)
class PROJECTOPENWORLD_API UPalHpWidget : public UUserWidget
{
	GENERATED_BODY()	
protected:
	// 몬스터/펠이 파괴돼도 안전하도록 약참조
	TWeakObjectPtr<UStatComponent> StatComponent{};

	UPROPERTY(EditAnywhere, Category = "PalStat")
	EStatusType Stat{};
	float GetHpPercent() const;

	virtual void UnbindStat() {}
	virtual void BindStat() {}
	virtual void SetupInfo(FName DisplayName, int32 Level) {}
public:
	// StatComponent와 표시 정보를 받아 바인딩까지 처리한다.
	void InitializeHPWidget(UStatComponent* InStatComponent, FName DisplayName, int32 Level);
};
