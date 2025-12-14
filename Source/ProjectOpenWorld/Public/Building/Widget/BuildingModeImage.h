#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "BuildingModeImage.generated.h"

class UStaticMesh;
class UBuildingModeWidget;

UCLASS()
class PROJECTOPENWORLD_API UBuildingModeImage : public UButton
{
	GENERATED_BODY()
public:
	UBuildingModeImage();
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BuildingMode")
	uint8 SlotIndex{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BuildingMode")
	TObjectPtr<UStaticMesh> BuildingMesh{};

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BuildingMode")
	TObjectPtr < UBuildingModeWidget> ParentWidget{};
	//DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(FEventReply, FOnPointerEvent, FGeometry, MyGeometry, const FPointerEvent&, MouseEvent);
	UFUNCTION()
	void OnStartBuilding();

	UFUNCTION()
	void OnSelectBuilding();
};
