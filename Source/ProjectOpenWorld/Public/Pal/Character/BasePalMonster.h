#pragma once

#include "CoreMinimal.h"
#include "Creature/Character/BaseMonster.h"
#include "BasePalMonster.generated.h"

class UPalPatrolComponent;

UCLASS()
class PROJECTOPENWORLD_API ABasePalMonster : public ABaseMonster
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal|Component")
	TObjectPtr<UPalPatrolComponent> PalPatrolComponent{};
public:
	ABasePalMonster();
	UFUNCTION(BlueprintPure, Category = "Pal|Component")
	FORCEINLINE UPalPatrolComponent* GetPalPatrolComponent() const { return PalPatrolComponent; }
	
};
