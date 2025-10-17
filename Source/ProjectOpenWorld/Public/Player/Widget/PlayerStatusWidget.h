#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatusWidget.generated.h"

class UPlayerStatusSlot;
class UPlayerStatusProgress;

UCLASS()
class PROJECTOPENWORLD_API UPlayerStatusWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	enum EStatusProgressType
	{
		ProgressNone,
		Hp,
		Shield,
		Health,
		ProgressMax
	};
	enum EStatusSlotType
	{
		SlotNone,
		MaxHp,
		Stamina,
		Attack,
		Defense,
		WorkSpeed,
		MaxWeight,
		SlotMax
	};
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr<UPlayerStatusProgress> ProgressHp{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr<UPlayerStatusProgress> ProgressShield{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr<UPlayerStatusProgress> ProgressHealth{};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr<UPlayerStatusSlot> SlotMaxHp{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr<UPlayerStatusSlot> SlotStamina{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr<UPlayerStatusSlot> SlotAttack{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr<UPlayerStatusSlot> SlotDefense{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr<UPlayerStatusSlot> SlotWorkSpeed{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr<UPlayerStatusSlot> SlotMaxWeight{};

public:
	UPlayerStatusWidget(const FObjectInitializer& ObjectInitializer);

	void SetStatusProgress(EStatusProgressType StatusType, float& Value, float& MaxValue);
	void SetStatusSlot(EStatusSlotType StatusType, float& Value);

protected:
	virtual void NativeConstruct() override;
};
