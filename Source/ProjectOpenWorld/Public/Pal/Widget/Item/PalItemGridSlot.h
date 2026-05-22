#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalItemGridSlot.generated.h"

struct FInventorySlot;
class UImage;
class UTextBlock;
class UPalItemInventoryWidget;

UCLASS(Abstract)
class PROJECTOPENWORLD_API UPalItemGridSlot : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, Category = "ItemSlot", meta = (BindWidget))
	TObjectPtr<UImage> ItemImage{};
	UPROPERTY(EditDefaultsOnly, Category = "ItemSlot", meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemCount{};
	UPROPERTY()
	TObjectPtr< UPalItemInventoryWidget> OwnerWidget{};
	bool bHasItem{};
public:
	void UpdateSlot(const FInventorySlot& ItemSlot);
	void InitializeSlot(UPalItemInventoryWidget* Widget);

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
