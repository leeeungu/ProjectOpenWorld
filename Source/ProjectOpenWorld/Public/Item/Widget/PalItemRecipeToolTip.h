#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalItemRecipeToolTip.generated.h"

class UVerticalBox;
class UTextBlock;
class UImage;

UCLASS(Abstract)
class PROJECTOPENWORLD_API UPalItemRecipeToolTip : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemRecipe")
	FName RecipeID = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr< UVerticalBox> MaterialList{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr <UTextBlock> RecipeNameText{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr <UTextBlock> RecipeDescText{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr <UImage> RecipeIconImage{};
protected:
	virtual void NativeConstruct() override;
public:
	UFUNCTION(BlueprintCallable, Category = "ItemRecipe")
	void SetRecipeID(FName InRecipeID);

	void UpdateToolTipWidget();
};
