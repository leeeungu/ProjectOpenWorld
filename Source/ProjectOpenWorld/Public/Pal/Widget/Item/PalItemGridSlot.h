#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalItemGridSlot.generated.h"

struct FInventorySlot;
class UImage;
class UTextBlock;
class UPalItemInventoryWidget;
class UInventorySlotWidget;
class UBaseItem;

UCLASS(Abstract)
class PROJECTOPENWORLD_API UPalItemGridSlot : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, Category = "ItemSlot", meta = (BindWidget))
	TObjectPtr<UInventorySlotWidget> InventorySlotWidget{};
	UPROPERTY(EditDefaultsOnly, Category = "ItemSlot", meta = (BindWidget))
	TObjectPtr< UImage> SlotFrame{};
	UPROPERTY()
	TObjectPtr< UPalItemInventoryWidget> OwnerWidget{};
	bool bHasItem{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	TObjectPtr<UBaseItem> ItemObj{};
public:
	void UpdateSlot(const FInventorySlot& ItemSlot);
	void InitializeSlot(UPalItemInventoryWidget* Widget);
	bool AddItemToInventory(const FInventorySlot& ItemSlot);
	UFUNCTION()
	void OnSlotRemoveEvent(UDragDropOperation* Operation);
	UBaseItem* GetSlotPtr() const { return ItemObj; }

	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
