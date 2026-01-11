#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalItemRecipeSlot.generated.h"

class UImage;
class UTextBlock;
class UButton;
class UPalItemWorkingWidget;

UCLASS()
class PROJECTOPENWORLD_API UPalItemRecipeSlot : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemRecipe")
	FName RecipeID = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "ItemRecipe")
	TObjectPtr<UImage> ItemIcon{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "ItemRecipe")
	TObjectPtr<UTextBlock> ItemCountText{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "ItemRecipe")
	TObjectPtr<UButton> ItemButton{};
	UPROPERTY()
	TObjectPtr<UPalItemWorkingWidget> OwningWorkingWidget{};
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnItemButtonClicked();
public:
	
	UFUNCTION(BlueprintCallable, Category = "ItemRecipe")
	void SetRecipeID(const FName& InRecipeID);

	//UImage
	//UTextBlock
	//UButton
};
