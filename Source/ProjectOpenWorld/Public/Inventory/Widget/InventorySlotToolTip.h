#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotToolTip.generated.h"

class UVerticalBox;
class UTextBlock;
class UImage;

UCLASS()
class PROJECTOPENWORLD_API UInventorySlotToolTip : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemRecipe")
	FName RecipeID = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr <UTextBlock> ItemNameText{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr <UTextBlock> ItemDescText{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr <UImage> ItemIconImage{};
protected:
	virtual void NativeConstruct() override;
public:
	UFUNCTION(BlueprintCallable, Category = "ItemRecipe")
	void SetRecipeID(FName InRecipeID);
};
