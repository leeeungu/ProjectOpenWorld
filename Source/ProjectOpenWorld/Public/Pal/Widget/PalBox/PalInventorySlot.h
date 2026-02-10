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
	TObjectPtr<UImage> SlotFrameImage{};
	TWeakObjectPtr<ABaseCreature> CurrentSelectedCreature{};

	FEventReply DragDropRelpy{};
	int SlotIndex{ -1 };
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	//
	//void UpdateSlot();
	void SetSlotIndex(int InSlotIndex) { SlotIndex = InSlotIndex; }
	void SetPalCreature(ABaseCreature* SelectedCreature);
protected:

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
