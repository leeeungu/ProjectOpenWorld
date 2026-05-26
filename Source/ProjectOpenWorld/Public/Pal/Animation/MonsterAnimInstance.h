#pragma once

#include "CoreMinimal.h"
#include "GameBase/Animation/BaseAnimInstance.h"
#include "Pal/Data/PalCommandData.h"
#include "MonsterAnimInstance.generated.h"

class APalBaseMonster;
class UPalCommandComponent;
class UAnimSequence;
class UPalAttackComponent;

UCLASS()
class PROJECTOPENWORLD_API UMonsterAnimInstance : public UBaseAnimInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, Category = "PalAnim")
	TObjectPtr<APalBaseMonster> OwnerPalCreature{};
	//UPROPERTY(BlueprintReadOnly, Category = "PalAnim")
	//TObjectPtr<UPalCommandComponent> CommandComponent{};

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAnim")
	//EPalCommandKind CurrentCommandKind{};
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAnim")
	//uint8 SubCommandType{};
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

};
