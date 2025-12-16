#pragma once

#include "CoreMinimal.h"
#include "Building/BuildingAssistComponent.h"
#include "BuildingAssistComponentV2.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UBuildingAssistComponentV2 : public UBuildingAssistComponent
{
	GENERATED_BODY()
protected:
	virtual bool UpdatePreview() override;
};
