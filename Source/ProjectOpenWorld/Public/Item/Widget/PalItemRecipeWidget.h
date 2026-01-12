#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalItemRecipeWidget.generated.h"

class UTextBlock;
class UImage;
class UVerticalBox;
class UButton;
class USlider;
class UEditableText;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "ItemRecipe")
	TObjectPtr<UButton> MakeButton{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "ItemRecipe")
	TObjectPtr<USlider> MakeItemCountSlider{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "ItemRecipe")
	TObjectPtr<UTextBlock> MakeItemMaxCountText{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "ItemRecipe")
	TObjectPtr<UTextBlock> MakeItemMinCountText{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "ItemRecipe")
	TObjectPtr<UEditableText> MakeItemCountText{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "ItemRecipe")
	TObjectPtr<UTextBlock> WorkingAmountText{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (ClampMin = "0"), Category = "ItemRecipe")
	int MakeItemCount{ 1 };

protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void OnSliderChanged(float InValue);

	UFUNCTION()
	void OnMakeButtonClicked();
	UFUNCTION()
	void OnChangeMakeItemCountText(const FText& InText);
public:
	UFUNCTION(BlueprintCallable, Category = "ItemMaterial")
	void SetItemRecipe( FName InItemRecipe);
};
