#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalItemRecipeWidget.generated.h"

class UTextBlock;
class UImage;
class UVerticalBox;

UCLASS()
class PROJECTOPENWORLD_API UPalItemRecipeWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMaterial")
	FName ItemRecipe_ID = NAME_None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemNameText{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> ItemIconImage{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr< UVerticalBox> MaterialList{};
protected:
	virtual void NativeOnInitialized() override;

public:
	UFUNCTION(BlueprintCallable, Category = "ItemMaterial")
	void SetItemRecipe(const FName& InItemRecipe);
};
