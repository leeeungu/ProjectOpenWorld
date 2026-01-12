#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalItemRecipeMaterialWidget.generated.h"

class UTextBlock;
class UImage;
class USizeBox;

UCLASS()
class PROJECTOPENWORLD_API UPalItemRecipeMaterialWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMaterial")
	FName Material_ID = NAME_None;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemMaterial")
	int Material_Count{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USizeBox> MaterialSizeBox{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemNameText{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr< UImage> ItemIconImage{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MaterialCountText{};

	

public:
	UFUNCTION(BlueprintCallable, Category = "ItemMaterial")
	void SetMaterialData( FName InMaterialID, int InMaterialCount);

	void SetWidgetSize(FVector2D InSize);

	void UpdateMaterialWidget();

	bool HasMaterial() const;
	FName GetMaterialID() const { return Material_ID; }
	int GetMaterialCount() const { return Material_Count; }
	int GetMakeableCount() const;
	
};
