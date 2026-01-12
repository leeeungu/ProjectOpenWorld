#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/Interface/InventorySlotInterface.h"
#include "InventorySlotWidget.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class PROJECTOPENWORLD_API UInventorySlotWidget : public UUserWidget, public IInventorySlotInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> ItemImage{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ItemWeight{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidgetOptional	))
	TObjectPtr<UTextBlock> ItemCount{};
	
public:
	virtual void SetSlotData_Implementation(const FInventorySlot& Data) override;
};
