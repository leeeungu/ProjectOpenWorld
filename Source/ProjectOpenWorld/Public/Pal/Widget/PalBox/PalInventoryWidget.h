#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalInventoryWidget.generated.h"

class UUniformGridPanel;
class UTextBlock;
class UPalInventorySlot;
class ABaseCreature;
class UPalBoxWidget;

UCLASS()
class PROJECTOPENWORLD_API UPalInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr<UUniformGridPanel> PalBoxGridPanel{};
	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr<UTextBlock> BoxName{};
	
	UPROPERTY(EditDefaultsOnly, Category = "PalBox")
	TSubclassOf<UPalInventorySlot> InventorySlotClass{};
	UPROPERTY(EditDefaultsOnly, Category = "PalBox", meta = (ClampMin = 1, ClampMax = 6))
	int32 NumColumns = 6;
	UPROPERTY(EditDefaultsOnly, Category = "PalBox", meta = (ClampMin = 1, ClampMax = 5))
	int32 NumRows = 5;
	UPROPERTY()
	TObjectPtr< UPalBoxWidget> ParentPalBoxWidget{};
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

	void UpdatePalInventory(int InventoryIndex, TObjectPtr<ABaseCreature> NewPal);
	void SetPalSlot();
};
