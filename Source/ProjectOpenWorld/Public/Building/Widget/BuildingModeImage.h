#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "BuildingModeImage.generated.h"

class UStaticMesh;
class UBuildingModeWidget;
class UTexture2D;
UCLASS()
class PROJECTOPENWORLD_API UBuildingModeImage : public UButton
{
	GENERATED_BODY()
public:
	UBuildingModeImage();
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "BuildingMode")
	uint8 SlotIndex{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BuildingMode")
	FName BuildObjectId{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BuildingMode")
	TObjectPtr<UStaticMesh> BuildingMesh{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BuildingMode")
	TObjectPtr<UTexture2D> BuildingTexture{};
protected:
	UPROPERTY(BlueprintReadOnly, Category = "BuildingMode")
	TObjectPtr < UBuildingModeWidget> ParentWidget{};
	//DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(FEventReply, FOnPointerEvent, FGeometry, MyGeometry, const FPointerEvent&, MouseEvent);
	UFUNCTION()
	void OnStartBuilding();

	UFUNCTION()
	void OnHoverBuilding();
	UFUNCTION()
	void OnUnHoverBuilding();

	void ChangeButtonImage();
public:
	void SetParent(UBuildingModeWidget* Parent);
	void SetSlotIndex(uint8 Index);

	void SetBuildingID(FName ID);
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
};
