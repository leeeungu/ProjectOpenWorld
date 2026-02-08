#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "BuildingActionWidgetComponent.generated.h"

UCLASS(ClassGroup = (Architecture), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UBuildingActionWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	UBuildingActionWidgetComponent();
	virtual void InitWidget() override;
};
