#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildingDescWidget.generated.h"

class UTextBlock;
class UVerticalBox;

UCLASS()
class PROJECTOPENWORLD_API UBuildingDescWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BuildingDesc")
	FName BuildObjectId{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildingDesc", meta = (BindWidget))
	TObjectPtr<UTextBlock> BuildNameText{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildingDesc", meta = (BindWidget))
	TObjectPtr<UTextBlock> BuildDescText{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildingDesc", meta = (BindWidget))
	TObjectPtr<UVerticalBox> RecipeMaterialVerticalBox{};


	void UpdateNameText();
	void UpdateDescText();
	void UpdateRecipeVericalBox();

public:
	void SetBuildingDescData(FName InBuildObjectId);

	
};
