#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/Interface/InventorySlotInterface.h"
#include "InventorySlotBase.generated.h"

class UCanvasPanel;
class UInventorySlotWidget;
class UInventorySlotToolTip;
class UImage;
class UInventoryComponent;

UCLASS()
class PROJECTOPENWORLD_API UInventorySlotBase : public UUserWidget, public IInventorySlotInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory", meta = (BindWidget))
	TObjectPtr<UCanvasPanel> inventorySlotCanvas{};

	TSoftObjectPtr<UInventorySlotWidget> inventorySlotUW{};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventorySlotToolTip> ToolTipWidgetClass{};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory", meta = (BindWidget))
	TObjectPtr<UImage> SlotFrame{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Inventory")
	int inventoryRow = -1;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Inventory")
	int inventoryCol = -1;

	const FInventorySlot* itemPointer{};
public:
	UInventorySlotBase(const FObjectInitializer& ObjectInitializer);
	virtual void SetSlotData_Implementation(const FInventorySlot& Data) override;
	virtual void SetSlotPtr(const FInventorySlot* Data) override;
	virtual void SetSlotIndex_Implementation(int Row, int Col) override;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SwapSlot(UInventorySlotBase* OtherSlot);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE UInventorySlotWidget* GetInventorySlotWidget() const;

protected:

	const FInventorySlot* GetInventorySlotData() const
	{
		return itemPointer;
	}
	bool SwapSlotPtr(const FInventorySlot* Dst);

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;


	UInventoryComponent* GetInventoryComponent() const;
};
