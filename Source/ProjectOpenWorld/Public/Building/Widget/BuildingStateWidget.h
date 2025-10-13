#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildingStateWidget.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UBuildingStateWidget : public UUserWidget
{
	GENERATED_BODY()
	const float* buildPercent = nullptr;
	const float* buildTime = nullptr;
	const float* buildSpeed = nullptr;
public:
	void SetBuildPercent(const float* BuildPercent);
	void SetBuildTime(const float* BuildTime);
	void SetBuildSpeed(const float* BuildSpeed);

	UFUNCTION(BlueprintPure, Category = "Building")
	FText GetBuildTime() const;
	UFUNCTION(BlueprintPure, Category = "Building")
	float GetBuildPercent() const { return buildPercent ? *buildPercent : 0.f; }


	UFUNCTION(BlueprintNativeEvent, Category = "Building")
	void OnBuildingEnd();
	virtual void OnBuildingEnd_Implementation() {}
};
