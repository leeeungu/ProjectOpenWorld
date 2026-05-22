#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalItemInventoryWidget.generated.h"

class UUniformGridPanel;
struct FInventorySlot;
class UPalItemGridSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWidgetOpen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotSwap, int32, Src, int32, Dst);

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

	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnUpdateSlot(int32 Index, const FInventorySlot& ItemSlot);
	void OnSlotSwapEvent(UWidget* pSrc, UWidget* pDst);
};
