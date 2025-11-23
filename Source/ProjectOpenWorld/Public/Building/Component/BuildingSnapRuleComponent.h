#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildingSnapRuleComponent.generated.h"

class UBuildingPreviewComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UBuildingSnapRuleComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TObjectPtr<UBuildingPreviewComponent> buildingPreviewActor{};

	UPROPERTY()
	TSoftObjectPtr < APawn> ownerPawn{};

public:
	UBuildingSnapRuleComponent();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	bool bSnapped{};
};