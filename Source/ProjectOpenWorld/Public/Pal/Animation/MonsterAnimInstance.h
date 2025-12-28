#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Data/PalCommandData.h"
#include "MonsterAnimInstance.generated.h"

class ABaseMonster;
class UCharacterMovementComponent;
class UPalCommandComponent;



//class MetaDataStartInterface
//{
//public:
//	UFUNCTION()
//	virtual void () = 0;
//};
//
//class MetaDataUpdateInterface
//{
//public:
//	UFUNCTION()
//	virtual void AnimNotify_AttackStart() = 0;
//};

UCLASS()
class PROJECTOPENWORLD_API UMonsterAnimInstance : public UAnimInstance//, public MetaDataStartInterface
{
	GENERATED_BODY()
	protected:
		UPROPERTY(BlueprintReadOnly, Category = "PalAnim")
		ABaseMonster* OwnerPalCreature{};
		UPROPERTY(BlueprintReadOnly, Category = "PalAnim")
		UCharacterMovementComponent* MovementComponent{};
		UPROPERTY(BlueprintReadOnly, Category = "PalAnim")
		UPalAttackComponent* AttackComponent{};
		UPROPERTY(BlueprintReadOnly, Category = "PalAnim")
		UPalCommandComponent* CommandComponent{};

		UPROPERTY(BlueprintReadOnly, Category = "PalAnim")
		FPalTempAttackAnim AttckAnimations{};

		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAnim")
		bool IsFalling{};
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAnim")
		float Speed{};
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAnim")
		EPalCommandKind CurrentCommandKind{};
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAnim")
		uint8 SubCommandType{};
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAnim")
		bool bAttacking{};
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAnim")
		bool bAttackLoop{};

		float CurrentTime{};

	public:
		virtual void NativeInitializeAnimation() override;
		virtual void NativeUpdateAnimation(float DeltaSeconds) override;

		UFUNCTION()
		void AnimNotify_AttackStart() ;
		void AnimNotify_TestStop();
		void AnimNotify_TestEnd() ;
	};
