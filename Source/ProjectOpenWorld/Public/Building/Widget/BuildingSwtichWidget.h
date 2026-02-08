#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildingSwtichWidget.generated.h"

class UBuildingModeWidget;
class UTextBlock;
class UImage;
class UButton;

UCLASS()
class PROJECTOPENWORLD_API UBuildingSwtichWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildingSwitch", meta = (BindWidget))
	TObjectPtr<UTextBlock> SwtichName{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildingSwitch", meta = (BindWidget))
	TObjectPtr<UImage> SwtichImage{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildingSwitch", meta = (BindWidget))
	TObjectPtr<UButton> SwtichButton{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BuildingSwitch")
	TArray<FName> BuildingIDs{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BuildingSwitch")
	FText SwitchDisplayName{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BuildingSwitch")
	TObjectPtr<UTexture2D> SwitchDisplayImage{};


	UPROPERTY(BlueprintReadOnly, Category = "BuildingMode")
	TObjectPtr < UBuildingModeWidget> ParentWidget{};
public:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;
protected:
	UFUNCTION()
	void OnSwtichClicked();
};
