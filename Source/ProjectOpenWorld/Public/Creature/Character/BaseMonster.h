#pragma once

#include "CoreMinimal.h"
#include "GameBase/BaseCharacter.h"
#include "GameBase/Interface/AttackInterface.h"
#include "BaseMonster.generated.h"

UCLASS()
class PROJECTOPENWORLD_API ABaseMonster : public ABaseCharacter , public IAttackInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	float Hp{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	float Attack{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	float Defend{};
public:
	ABaseMonster();

public:
	 // //virtual bool Attacked_Implementation(IAttackInterface* Other) override;
	 // virtual float GetAttackValue_Implementation() const override;
};
