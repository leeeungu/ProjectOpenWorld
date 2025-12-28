#include "Pal/Animation/MonsterAnimInstance.h"
#include "Creature/Character/BaseMonster.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/Component/PalCommandComponent.h"
#include "GameBase/MetaData/AnimMetaData_LoopData.h"

void UMonsterAnimInstance::NativeInitializeAnimation()
{
	UAnimInstance::NativeInitializeAnimation();
	OwnerPalCreature = Cast<ABaseMonster>(TryGetPawnOwner());
	if (!OwnerPalCreature)
		return;
	MovementComponent = OwnerPalCreature->GetCharacterMovement();
	AttackComponent = OwnerPalCreature->GetAttackComponent();
	CommandComponent = OwnerPalCreature->GetCommandComponent();


}

void UMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	UAnimInstance::NativeUpdateAnimation(DeltaSeconds);
	if (!OwnerPalCreature || !MovementComponent || !CommandComponent || !AttackComponent)
	{
		return;
	}
	IsFalling = MovementComponent->IsFalling();
	Speed = 0.0f;
	if (MovementComponent->GetMaxSpeed() > 0)
		Speed = MovementComponent->Velocity.Length() / MovementComponent->GetMaxSpeed();
	Speed = FMath::Clamp(Speed, 0.0f, 1.0f);
	CurrentCommandKind = CommandComponent->GetCurrentCommand_C()->CommandKind;
	SubCommandType = CommandComponent->GetCurrentCommand_C()->SubCommandType;
	AttckAnimations = AttackComponent->GetAnimation();
	bAttacking = AttackComponent->GetAttacking();

	if (AttckAnimations.Loop )
	{
		if (bAttackLoop)
		{
			UAnimMetaData_LoopData* Data = AttckAnimations.Loop->FindMetaDataByClass<UAnimMetaData_LoopData>();
			if (Data)
			{
				if (CurrentTime >= 0)
				{
					CurrentTime -= DeltaSeconds;
					if (CurrentTime < 0)
					{
						SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
						IAnimMetaDataUpdateInterface::Execute_InitUpdateMetaData(Data, OwnerPalCreature, AttckAnimations.Loop->GetPlayLength());
					}
				}
				else
				{
					IAnimMetaDataUpdateInterface::Execute_UpdateMetaData(Data, DeltaSeconds);
					if (IAnimMetaDataUpdateInterface::Execute_IsLoopingEnd(Data))
					{
						bAttackLoop = false;
						SetRootMotionMode(ERootMotionMode::RootMotionFromEverything);
						OwnerPalCreature->GetCharacterMovement()->bUseControllerDesiredRotation = true;
					}
				}

			}
		}
	}
}

void UMonsterAnimInstance::AnimNotify_AttackStart()
{
	if (!AttckAnimations.Start)
		return;
	CurrentTime = AttckAnimations.Start->GetPlayLength();
	bAttackLoop = true;
}

void UMonsterAnimInstance::AnimNotify_TestStop()
{
	//IsPlayingSlotAnimation
}

void UMonsterAnimInstance::AnimNotify_TestEnd()
{
	//if (!AttckAnimations.Loop)
	//	return;
	//UAnimMetaData_LoopData* Data = AttckAnimations.Loop->FindMetaDataByClass<UAnimMetaData_LoopData>();
	//if (!Data)
	//	return;
	//Data->DataEnd();
}
