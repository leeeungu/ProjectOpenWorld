#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildingInfoWidget.generated.h"

class UHorizontalBox;
class UTextBlock;


UCLASS()
class PROJECTOPENWORLD_API UBuildingInfoWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildingInfo")
	FName BuildObjectId{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildingInfo", meta = (BindWidget))
	TObjectPtr<UTextBlock> BuildObjectIdText{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildingInfo", meta = (BindWidget))
	TObjectPtr<UHorizontalBox> MaterialBox{};
public:
	UFUNCTION(BlueprintCallable, Category = "BuildingInfo")
	void SetBuildingInfoData(FName InBuildObjectId);

	void UpdateBuildingInfo();

};
