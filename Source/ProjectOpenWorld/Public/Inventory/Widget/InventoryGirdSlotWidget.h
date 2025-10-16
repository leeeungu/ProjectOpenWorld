#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/Interface/InventorySlotInterface.h"
#include "InventoryGirdSlotWidget.generated.h"

class UCanvasPanel;
class UInventorySlotWidget;

UCLASS()
class PROJECTOPENWORLD_API UInventoryGirdSlotWidget : public UUserWidget , public IInventorySlotInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Inventory", meta = (BindWidget))
	TObjectPtr<UCanvasPanel> inventorySlotCanvas{};

	TSubclassOf<UInventorySlotWidget> inventorySlotClass{};
	TSoftObjectPtr<UInventorySlotWidget> inventorySlotUW{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Inventory")
	int inventoryRow{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Inventory")
	int inventoryCol{};
	const FInventorySlot* itemPointer{};
public:
	UInventoryGirdSlotWidget(const FObjectInitializer& ObjectInitializer);
	virtual void SetSlotData_Implementation(const FInventorySlot& Data) override;
	virtual void SetSlotIndex_Implementation(int Row, int Col) override;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SwapSlot(UInventoryGirdSlotWidget* OtherSlot);
	bool SwapSlot_Index(int Row, int Col);
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE UInventorySlotWidget* GetInventorySlotWidget() const;
protected:
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
