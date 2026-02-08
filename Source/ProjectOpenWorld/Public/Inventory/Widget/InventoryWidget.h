#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UUniformGridPanel;
class UInventoryComponent;
class UInventoryGirdSlotWidget;
class UProgressBar;
class UTextBlock;

UCLASS()
class PROJECTOPENWORLD_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory", meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> inventoryGridPanel{};
	TSoftObjectPtr<UInventoryComponent> inventoryComponent{};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventoryGirdSlotWidget> inventorySlotClass{};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory", meta = (BindWidget))
	TObjectPtr<UProgressBar> WeightProgressBar{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory", meta = (BindWidget))
	TObjectPtr<UTextBlock> WeightPercentTextBlock{};

public:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	void UpdateInventorySlot(int Row, int Col);

private:
	UFUNCTION()
	void UpdateAllInventorySlot();
	void UpdateInventoryWeight();

};
