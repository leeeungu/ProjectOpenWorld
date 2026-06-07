#pragma once

#include "CoreMinimal.h"
#include "Creature/Character/BaseMonster.h"
#include "BasePalMonster.generated.h"

class UPalPatrolComponent;

UCLASS(
	Abstract,                  // SpawnActor / NewObject 불가
	NotBlueprintable,          // BP 자식 클래스 못 만듦
	NotPlaceable,              // 레벨 드래그 배치 못함
	HideDropdown,              // 클래스 드롭다운에서 숨김
	meta = (
		DeprecationMessage = "Use APalBaseMonster instead. This class will be removed."
		)
)
class  PROJECTOPENWORLD_API ABasePalMonster : public ABaseMonster
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
