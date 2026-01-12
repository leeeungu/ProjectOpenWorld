#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalBuildingRecipeMaterial.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class PROJECTOPENWORLD_API UPalBuildingRecipeMaterial : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaterialData")
	FName MaterialId{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaterialData")
	int32 MaterialCount = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MaterialData", meta = (BindWidget))
	TObjectPtr<UTextBlock> MaterialCountText{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MaterialData", meta = (BindWidget))
	TObjectPtr<UImage> MaterialIconImage{};

public:
	UFUNCTION(BlueprintCallable, Category = "Building")
	void SetMaterialData(FName InMaterialId, int32 InMaterialCount);

	void UpdateMaterialData();
};
