#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/Interface/MainWidgetInterface.h"
#include "PlayerInventoryWidget.generated.h"

class UEquipSlotWidget;
class UInventorySlotBase;
class UStatComponent;
class UPlayerStatusWidget;
class UPalStorageComponent;
class UPalInventoryWidget;

UCLASS()
class PROJECTOPENWORLD_API UPlayerInventoryWidget : public UUserWidget, public IMainWidgetInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory", meta = (BindWidget))
	TObjectPtr < UEquipSlotWidget> WeaponeSlot{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory", meta = (BindWidget))
	TObjectPtr < UEquipSlotWidget> AxeSlot{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory", meta = (BindWidget))
	TObjectPtr < UEquipSlotWidget> PickAxeSlot{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory", meta = (BindWidget))
	TObjectPtr < UInventorySlotBase> HeadEquipSlot{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory", meta = (BindWidget))
	TObjectPtr < UInventorySlotBase> BodyEquipSlot{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory", meta = (BindWidget))
	TObjectPtr < UPlayerStatusWidget> PlayerStatusWidget{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory", meta = (BindWidget))
	TObjectPtr < UPalInventoryWidget> PalInventoryWidget{};
public:
	// IMainWidgetInterface interface
	virtual bool SetMainWidget() override;
	virtual void UnSetMainWidget() override;


	void SetStatWidget(UStatComponent* StatCom);
	void SetStorageComponent(UPalStorageComponent* Storage);
protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	UFUNCTION()
	void UpdatEquipSlot();
};
