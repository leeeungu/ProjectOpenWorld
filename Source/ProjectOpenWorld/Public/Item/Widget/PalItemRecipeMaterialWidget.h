#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalItemRecipeMaterialWidget.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class PROJECTOPENWORLD_API UPalItemRecipeMaterialWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMaterial")
	FName Material_ID{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemMaterial")
	int Material_Count{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemNameText{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr< UImage> ItemIconImage{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MaterialCountText{};

public:
	UFUNCTION(BlueprintCallable, Category = "ItemMaterial")
	void SetMaterialData(const FName& InMaterialID, int InMaterialCount);
};
