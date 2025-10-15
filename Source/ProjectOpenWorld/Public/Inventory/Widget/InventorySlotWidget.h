#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/Interface/InventorySlotInterface.h"
#include "InventorySlotWidget.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UInventorySlotWidget : public UUserWidget, public IInventorySlotInterface
{
	GENERATED_BODY()

public:
	virtual void SetSlotData_Implementation(const FInventorySlot& Data) override {}
};
