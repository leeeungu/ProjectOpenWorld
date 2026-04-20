#pragma once

#include "CoreMinimal.h"
#include "Inventory/Widget/InventorySlotBase.h"
#include "InventoryGirdSlotWidget.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UInventoryGirdSlotWidget : public UInventorySlotBase
{
	GENERATED_BODY()
protected:

	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Inventory")
	//bool bIsEquippedSlot{};

public:
	//UInventoryGirdSlotWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

};
