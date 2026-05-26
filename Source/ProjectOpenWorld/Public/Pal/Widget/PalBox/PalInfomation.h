#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalInfomation.generated.h"

class APalBaseCreature;
class UTextBlock;
class UImage;
class UProgressBar;
class UPalInventorySlot;
class UVerticalBox;

UCLASS(ClassGroup = PalBox)
class PROJECTOPENWORLD_API UPalInfomation : public UUserWidget
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
	TObjectPtr<UProgressBar> PalHealthBar{};

	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr<UVerticalBox> InfoVerticalBox{};

	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, Category = "PalBox")
	TObjectPtr<UPalInventorySlot> PalInventorySlot{};

	UPROPERTY(VisibleAnywhere, Category = "PalBox")
	TWeakObjectPtr<APalBaseCreature> CurrentSelectedCreature{};
public:
	virtual void NativeConstruct() override;

	void SetPalCreature(APalBaseCreature* SelectedCreature);
};
