#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalItemRecipeSlot.generated.h"

class UImage;
class UTextBlock;
class UButton;
class UPalItemWorkingWidget;
class UPalItemRecipeToolTip;

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
	TObjectPtr<UImage> SlotFrame{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "ItemRecipe")
	TObjectPtr<UTextBlock> ItemCountText{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "ItemRecipe")
	TObjectPtr<UButton> ItemButton{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = "ItemRecipe")
	TSubclassOf<UPalItemRecipeToolTip> ToolTipWidgetClass{};
	UPROPERTY()
	TObjectPtr<UPalItemWorkingWidget> OwningWorkingWidget{};
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	UFUNCTION()
	void OnItemButtonClicked();
	UFUNCTION()
	void OnItemButtonHovered();
	UFUNCTION()
	void OnItemButtonUnhovered();
public:
	FName GetRecipeID() const { return RecipeID; }
	
	UFUNCTION(BlueprintCallable, Category = "ItemRecipe")
	void SetRecipeID( FName InRecipeID);

	//UImage
	//UTextBlock
	//UButton
};
