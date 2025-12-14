#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildingModeWidget.generated.h"

class UBuildingAssistComponent;
class UImage;

UCLASS()
class PROJECTOPENWORLD_API UBuildingModeWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, Category = "BuildingAssist")
	TObjectPtr<UBuildingAssistComponent> BuildingAssistComp{};
	UStaticMesh* CurrentHoverMesh = nullptr;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (Bindwidget))
	UImage* SelectImage = nullptr;
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
public:
	UFUNCTION()
	void StartBuildingMode();
	UFUNCTION()
	void HoverBuildingMode(int SlotIndex, UStaticMesh* Mesh);
};
