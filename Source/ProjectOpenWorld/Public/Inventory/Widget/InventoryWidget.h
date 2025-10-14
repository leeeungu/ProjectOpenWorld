#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UUniformGridPanel;
class UInventoryComponent;
class UInventorySlotWidget;

UCLASS()
class PROJECTOPENWORLD_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Inventory", meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> inventoryGridPanel{};
	TSoftObjectPtr<UInventoryComponent> inventoryComponent{};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventorySlotWidget> inventorySlotClass;
public:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	void UpdateInventorySlot(int Row, int Col);
};
