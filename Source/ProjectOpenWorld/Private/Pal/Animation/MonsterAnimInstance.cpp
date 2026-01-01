#include "Pal/Animation/MonsterAnimInstance.h"
#include "Creature/Character/BaseMonster.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/Component/PalCommandComponent.h"
#include "GameBase/MetaData/AnimMetaData_LoopData.h"

void UMonsterAnimInstance::StartMontage(UAnimMontage* Montage)
{
	if (Montage)
		UE_LOG(LogTemp, Warning, TEXT("StartMontage %s"), *Montage->GetName());
	if (CurrentMontage == Montage)
		return;
	CurrentMontage = Montage;
	LoopMetaData = nullptr;
	LoopCount = 0;
	if (CurrentMontage)
	{
		if (UAnimMetaData_LoopData* LoopData = CurrentMontage->FindMetaDataByClass<UAnimMetaData_LoopData>())
		{
			if (LoopData->GetLoopCount()  - 1> 0)
			{
				IAnimMetaDatLoopInterface::Execute_InitLoopMetaData(LoopData, OwnerPalCreature,
					0.0f, CurrentMontage->GetPlayLength());
				LoopCount = LoopData->GetLoopCount() - 1;
				LoopMetaData = LoopData;
			}
		}
	}
}

void UMonsterAnimInstance::MontageBlendingEvent(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage)
		UE_LOG(LogTemp, Warning, TEXT("MontageBlendingEvent %s"), *Montage->GetName());
	if (CurrentMontage == Montage && LoopMetaData)
	{
		IAnimMetaDatLoopInterface::Execute_LoopUpdate(LoopMetaData, 0.0f);
		if (!IAnimMetaDatLoopInterface::Execute_IsLoopingEnd(LoopMetaData))
		{
			Montage_Play(Montage);
		}
		else
		{
			Montage_Stop(0.2f);
			//bMontageLoop = false;
		}
	}
	else
	{
	
	}
}

void UMonsterAnimInstance::NativeInitializeAnimation()
{
	UAnimInstance::NativeInitializeAnimation();
	OwnerPalCreature = Cast<ABaseMonster>(TryGetPawnOwner());
	if (!OwnerPalCreature)
		return;
	MovementComponent = OwnerPalCreature->GetCharacterMovement();
	AttackComponent = OwnerPalCreature->GetAttackComponent();
	CommandComponent = OwnerPalCreature->GetCommandComponent();

	OnMontageBlendingOut.AddDynamic(this, &UMonsterAnimInstance::MontageBlendingEvent);
	OnMontageStarted.AddDynamic(this, &UMonsterAnimInstance::StartMontage);
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
	//AttckAnimations = AttackComponent->GetAttackAnimation();
	bAttacking = AttackComponent->GetAttacking();

	/*if (AttckAnimations)
	{
		UAnimMetaData_LoopData* Data = AttckAnimations->FindMetaDataByClass<UAnimMetaData_LoopData>();

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
	}*/
}

void UMonsterAnimInstance::AnimNotify_AttackStart()
{
	//if (!AttckAnimations)
	//	return;
	////UAnimMetaData_LoopData* Data = AttckAnimations->FindMetaDataByClass<UAnimMetaData_LoopData>();
	//if (Data && Data->GetLoopCount() > 0)
	//{
	//	//float StartLength = AttckAnimations.Start->GetPlayLength();
	//	float LoopLength = AttckAnimations->GetPlayLength();
	//	IAnimMetaDatLoopInterface::Execute_InitLoopMetaData(Data, OwnerPalCreature,
	//		0.0f, LoopLength);
	//	bAttackLoop = true; // IAnimMetaDatLoopInterface::Execute_IsLoopingEnd(Data);
	//	//AttackComponent->
	//}
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
