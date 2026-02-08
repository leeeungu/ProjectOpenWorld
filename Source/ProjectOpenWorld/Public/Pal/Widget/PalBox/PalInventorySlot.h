#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalInventorySlot.generated.h"

class UImage;
class UButton;
class ABaseCreature;

UCLASS(ClassGroup = PalBox)
class PROJECTOPENWORLD_API UPalInventorySlot : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr<UImage> SlotImage{};
	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr<UButton> SlotButton{};

	TWeakObjectPtr<ABaseCreature> CurrentSelectedCreature{};
public:
	virtual void NativeOnInitialized() override;

	void SetPalCreature(ABaseCreature* SelectedCreature);

protected:
	UFUNCTION()
	void OnSlotButtonClicked();

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
