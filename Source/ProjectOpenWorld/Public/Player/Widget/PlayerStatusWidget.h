#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatusWidget.generated.h"

class UPlayerStatusSlot;
class UPlayerStatusProgress;
class UStatComponent;
class UVerticalBox;

UCLASS()
class PROJECTOPENWORLD_API UPlayerStatusWidget : public UUserWidget
{
	GENERATED_BODY()
public:
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
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr<UPlayerStatusProgress> ProgressHp{};
	//UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr<UPlayerStatusProgress> ProgressShield{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr<UPlayerStatusProgress> ProgressHealth{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr<UVerticalBox> StatusView{};
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "PlayerStatus")
	TArray<TObjectPtr<UPlayerStatusSlot>> SlotArray{};
public:
	void SetStatWidget(UStatComponent* StatCom);
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

};
