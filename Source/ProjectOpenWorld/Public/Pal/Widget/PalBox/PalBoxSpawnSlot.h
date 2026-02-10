#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalBoxSpawnSlot.generated.h"

class UTextBlock;
class UImage;
class UProgressBar;
class ABaseCreature;
class UCanvasPanel;

UCLASS()
class PROJECTOPENWORLD_API UPalBoxSpawnSlot : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta = (BindWidget), Category = "PalBox", EditDefaultsOnly)
	TObjectPtr<UTextBlock> NameText = nullptr;
	UPROPERTY(meta = (BindWidget), Category = "PalBox", EditDefaultsOnly)
	TObjectPtr<UTextBlock> LevelText = nullptr;
	UPROPERTY(meta = (BindWidget), Category = "PalBox", EditDefaultsOnly)
	TObjectPtr<UImage> IconImage = nullptr;
	UPROPERTY(meta = (BindWidget), Category = "PalBox", EditDefaultsOnly)
	TObjectPtr<UImage> BackgroundFrameImage = nullptr;
	UPROPERTY(meta = (BindWidget), Category = "PalBox", EditDefaultsOnly)
	TObjectPtr<UImage> BackgroundImage = nullptr;
	UPROPERTY(meta = (BindWidget), Category = "PalBox", EditDefaultsOnly)
	TObjectPtr<UProgressBar> HpProgressBar = nullptr;
	UPROPERTY(meta = (BindWidget), Category = "PalBox", EditDefaultsOnly)
	TObjectPtr<UCanvasPanel> PalInfoCanvasPanel = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "PalBox")
	TWeakObjectPtr<ABaseCreature> CurrentSelectedCreature{};

	int SlotIndex{ -1 };
public:
	virtual void NativeConstruct() override;	
	virtual void NativeDestruct() override;	
	void SetPalCreature(ABaseCreature* InCreature);
	void SetSlotIndex(int InIndex) { SlotIndex = InIndex; }

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
