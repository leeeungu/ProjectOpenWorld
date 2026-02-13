#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameBase/Interface/ConfirmationInterface.h"
#include "Inventory/Interface/InventorySlotInterface.h"
#include "InventorySlotButton.generated.h"

class UTextBlock;
class UConfirmationWidgetBase;

UCLASS()
class PROJECTOPENWORLD_API UInventorySlotButton : public UUserWidget, public IInventorySlotInterface, public IConfirmationInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventoryButton", meta = (BindWidget))
	TObjectPtr<UTextBlock> ButtonText{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InventoryIndex")
	int SlotRow{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InventoryIndex")
	int SlotCol{};

	const FInventorySlot* SlotData{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ConfrirmationWidget")
	TSubclassOf<UConfirmationWidgetBase> ConfrirmationWidget{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ConfrirmationWidget")
	FText AlertMessage{};

public:
	UInventorySlotButton(const FObjectInitializer& ObjectInitializer);

	virtual void SetSlotData_Implementation(const FInventorySlot& Data) override;
	virtual void SetSlotIndex_Implementation(int Row, int Col) override;

	virtual void ConfirmationEvent_Implementation(bool IsConfirmation) override {}

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
protected:

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
