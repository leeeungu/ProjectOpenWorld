#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalInventoryWidget.generated.h"

class UUniformGridPanel;
class UTextBlock;
class UPalInventorySlot;
class APalBaseCreature;
class UPalBoxWidget;
class UPalStorageComponent;
class UPalBoxDDO;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryDropEvent, int32, SlotIndex, UPalBoxDDO*, PalBoxDDO);

UCLASS()
class PROJECTOPENWORLD_API UPalInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	TWeakObjectPtr<UPalStorageComponent> PalStorageComponent{};
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
	//UPROPERTY()
	//TObjectPtr< UPalBoxWidget> ParentPalBoxWidget{};

	UPROPERTY(EditAnywhere, Category = "Target")
	TArray<TObjectPtr<UWidget>> SelectEvent{};
public:
	FOnInventoryDropEvent OnInventoryDropEvent{};

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

	void UpdatePalInventory(int InventoryIndex, TObjectPtr<APalBaseCreature> NewPal);
	void SetPalSlot();

	void SetStorageComponent(UPalStorageComponent* Storage);
	UPalStorageComponent* GetPalStorageComponent() const;
	void OnSlotOnDropEvent(int32 SlotIndex, UPalBoxDDO* PalBoxDDO);
protected:
	UFUNCTION()
	void OnSlotSelected(int32 Index, AActor* SelectedCreature);
	UFUNCTION()
	void OnPalInventoryChanged(int nIndex, AActor* Actor);
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
