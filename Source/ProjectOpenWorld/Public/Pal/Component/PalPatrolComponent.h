#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PalPatrolComponent.generated.h"

struct FPalMonsterPatrolData;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalPatrolComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	const FPalMonsterPatrolData* PatrolDataPtr = nullptr;
	int CurrentPatrolIndex = 0;
public:	
	UPalPatrolComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetPatrolData(const FPalMonsterPatrolData* InPatrolData);
	void UpdatePatrolIndex();
	FVector GetCurrentPatrolPoint() const;
};
