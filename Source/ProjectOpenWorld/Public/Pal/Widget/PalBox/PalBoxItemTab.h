#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalBoxItemTab.generated.h"

class UInventoryWidget;
class UPalItemInventoryWidget;

UCLASS()
class PROJECTOPENWORLD_API UPalBoxItemTab : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (BindWidget))
	TObjectPtr<UInventoryWidget> InventoryWidget{};
	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (BindWidget))
	TObjectPtr<UPalItemInventoryWidget> PalItemInventoryWidget{};
public:
	UPalItemInventoryWidget* GetPalItemInventoryWidget() const { return PalItemInventoryWidget; }
};
