#pragma once

#include "CoreMinimal.h"
#include "Components/CanvasPanel.h"
#include "BuildingModeCanvasPanel.generated.h"

class UBuildingModeImage;

UCLASS()
class PROJECTOPENWORLD_API UBuildingModeCanvasPanel : public UCanvasPanel
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BuildingMode")
	uint8 SlotMaxIndex = -1;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BuildingMode")
	float Radius{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BuildingMode")
	FVector2D ButtonSize{};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BuildingMode")
	bool bCenterStart{};
	TArray< TObjectPtr<UBuildingModeImage>> arrImage{};
public:
	void PreConstruct();
	
};
