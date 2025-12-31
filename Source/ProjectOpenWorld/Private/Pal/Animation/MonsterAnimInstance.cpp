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

	if (AttckAnimations.Loop)
	{
		UAnimMetaData_LoopData* Data = AttckAnimations.Loop->FindMetaDataByClass<UAnimMetaData_LoopData>();

		if (Data)
		{
			if (bAttackLoop)
			{
				IAnimMetaDatLoopInterface::Execute_LoopUpdate(Data, DeltaSeconds);
				if (IAnimMetaDatLoopInterface::Execute_IsLoopingEnd(Data))
				{
					bAttackLoop = false;
				}
			}
		}
	}
}

void UMonsterAnimInstance::AnimNotify_AttackStart()
{
	if (!AttckAnimations.Loop || !AttckAnimations.Start)
		return;
	UAnimMetaData_LoopData* Data = AttckAnimations.Loop->FindMetaDataByClass<UAnimMetaData_LoopData>();
	if (Data && Data->GetLoopCount() > 0)
	{
		float StartLength = AttckAnimations.Start->GetPlayLength();
		float LoopLength = AttckAnimations.Loop->GetPlayLength();
		IAnimMetaDatLoopInterface::Execute_InitLoopMetaData(Data, OwnerPalCreature,
			StartLength, LoopLength);
		bAttackLoop = true; // IAnimMetaDatLoopInterface::Execute_IsLoopingEnd(Data);
		//AttackComponent->
	}
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
