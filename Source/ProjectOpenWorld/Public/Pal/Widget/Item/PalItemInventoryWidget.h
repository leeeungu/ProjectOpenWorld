#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalItemInventoryWidget.generated.h"

class UUniformGridPanel;
struct FInventorySlot;
class UPalItemGridSlot;
class UBaseItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWidgetOpen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotSwap, int32, Src, int32, Dst);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotUpdate, int32, Index);
DECLARE_DELEGATE_RetVal_OneParam(bool, FOnAddItem, UBaseItem*);

UCLASS()
class PROJECTOPENWORLD_API UPalItemInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> inventoryGridPanel{};
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 Row{};
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 Col{};
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UPalItemGridSlot> SlotClass{};
	
public:
	FOnWidgetOpen OnWidgetOpen{};
	FOnSlotSwap OnSlotSwap{};
	FOnSlotUpdate OnSlotUpdate{};
	FOnSlotUpdate OnSlotRemove{};
	FOnAddItem OnAddItem{};

	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnUpdateSlot(int32 Index, const FInventorySlot& ItemSlot);
	void OnSlotSwapEvent(UWidget* pSrc, UWidget* pDst);
	void OnSlotUpdateEvent(UWidget* pChild);
	void OnSlotRemoveEvent(UWidget* pChild);
	bool OnAddItemEvent(UBaseItem* ItemData);
};
