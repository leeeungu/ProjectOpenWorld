#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Pal/Interface/PalSlotSelectedInterface.h"
#include "PalInfomation.generated.h"

class APalBaseCreature;
class UTextBlock;
class UImage;
class UProgressBar;
class UPalInventorySlot;
class UVerticalBox;
class UStatusBarWidget;

UCLASS(ClassGroup = PalBox)
class PROJECTOPENWORLD_API UPalInfomation : public UUserWidget, public IPalSlotSelectedInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr<UTextBlock> PalLevelText{};
	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr<UTextBlock> PalNameText{};
	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr<UImage> PalImage{};
	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr<UStatusBarWidget> PalHealthBar{};

	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr<UVerticalBox> InfoVerticalBox{};

	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr<UPalInventorySlot> PalInventorySlot{};

	UPROPERTY(VisibleAnywhere, Category = "PalBox")
	TWeakObjectPtr<APalBaseCreature> CurrentSelectedCreature{};
public:
	virtual void NativeConstruct() override;

	void SetPalCreature(APalBaseCreature* SelectedCreature);


	virtual void OnSlotSelected_Implementation(int32 Index, AActor* SelectedCreature) override;
};
