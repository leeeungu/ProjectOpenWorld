#pragma once

#include "CoreMinimal.h"
#include "Building/Widget/BaseBuildingAction.h"
#include "PalItemWorkingWidget.generated.h"

class UUniformGridPanel;
class UPalItemRecipeWidget;
class UPalItemRecipeSlot;
	
UCLASS()
class PROJECTOPENWORLD_API UPalItemWorkingWidget : public UBaseBuildingAction
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "ItemRecipe")
	TObjectPtr<UUniformGridPanel> RecipeUniformGrid{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "ItemRecipe")
	TObjectPtr<UPalItemRecipeWidget> SelectRecipeWidget{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemRecipe")
	TArray<FName> RecipeIDList{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemRecipe")
	TSubclassOf<UPalItemRecipeSlot> RecipeWidgetClass{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemRecipe")
	int32 MaxRecipePerRow{ 8 };
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemRecipe")
	int32 MaxRecipePerCol{ 5 };


protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable, Category = "ItemRecipe")
	void SetSelectedRecipeWidget(FName SelectRecipeID);

};
