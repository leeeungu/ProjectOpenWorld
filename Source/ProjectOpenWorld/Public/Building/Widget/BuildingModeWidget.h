#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildingModeWidget.generated.h"

class UBuildingAssistComponent;
class UImage;
class UMaterialInstanceDynamic;
class UBuildingModeCanvasPanel;
class UBuildingDescWidget;

UCLASS()
class PROJECTOPENWORLD_API UBuildingModeWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, Category = "BuildingAssist")
	TObjectPtr<UBuildingAssistComponent> BuildingAssistComp{};
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (Bindwidget))
	TObjectPtr<UImage> SelectImage{};
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (Bindwidget))
	TObjectPtr<UBuildingModeCanvasPanel> ButtonPanel{};
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (Bindwidget))
	TObjectPtr<UBuildingDescWidget> BuildingDescWidget{};

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial{};

	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;



public:
	void SetBuildingUI(TArray<FName> BuildingIDs);
	void SelectBuilding(FName BuildingID);
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void StartViewWidget();
	void EndViewWidget();

	UFUNCTION()
	void StartBuildingMode(FName BuildingID , UStaticMesh* Mesh);
	UFUNCTION()
	void HoverBuildingMode(int SlotIndex);
};
