#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatusWidget.generated.h"

class UPlayerStatSlot;
class UPlayerStatProgress;

UCLASS()
class PROJECTOPENWORLD_API UPlayerStatusWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PlayerStat", meta = (BindWidgetOption))
	TObjectPtr<UPlayerStatProgress> ProgressShield{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PlayerStat", meta = (BindWidgetOption))
	TObjectPtr<UPlayerStatProgress> ProgressHp{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PlayerStat", meta = (BindWidgetOption))
	TObjectPtr<UPlayerStatProgress> ProgressHealth{};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PlayerStat", meta = (BindWidgetOption))
	TObjectPtr<UPlayerStatSlot> SlotMaxHp{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PlayerStat", meta = (BindWidgetOption))
	TObjectPtr<UPlayerStatSlot> SlotStamina{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PlayerStat", meta = (BindWidgetOption))
	TObjectPtr<UPlayerStatSlot> SlotAttack{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PlayerStat", meta = (BindWidgetOption))
	TObjectPtr<UPlayerStatSlot> SlotDefense{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PlayerStat", meta = (BindWidgetOption))
	TObjectPtr<UPlayerStatSlot> SlotWorkSpeed{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PlayerStat", meta = (BindWidgetOption))
	TObjectPtr<UPlayerStatSlot> SlotMaxWeight{};

public:
	enum EStatusType
	{
		NONE UMETA(Hidden),
		Hp,
		MaxHp,
		Shield,
		MaxShield,
		Health,
		MaxHealth,
		
		Stamina,
		Attack,
		Defense,
		WorkSpeed,
		MaxWeight,
	};


	void SetMaxHP(EStatusType StatType, float* Value) {}
};
