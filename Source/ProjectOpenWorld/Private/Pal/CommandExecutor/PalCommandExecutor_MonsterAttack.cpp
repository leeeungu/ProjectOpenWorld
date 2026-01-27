#include "Pal/CommandExecutor/PalCommandExecutor_MonsterAttack.h"
#include "Creature/Character/BaseMonster.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Controller/PalAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Pal/Component/PalCommandComponent.h"

void UPalCommandExecutor_MonsterAttack::Initialize(UPalCommandComponent* CommandComp)
{
	UPalCommandExecutorBase::Initialize(CommandComp);
	if (OwnerCommandComp == nullptr)
		return;
	OwnerPal = Cast<ABaseMonster>(OwnerCommandComp->GetOwner());
	if (OwnerPal)
	{
		AttackComponent = OwnerPal->GetAttackComponent();
		OwnerController = Cast<APalAIController>(OwnerPal->GetController());
	}
	if (AttackComponent)
	{
		AttackComponent->OnPalAttackEnd.AddUniqueDynamic(this, &UPalCommandExecutor_MonsterAttack::EndAttack);
	}
}


bool UPalCommandExecutor_MonsterAttack::StartCommand(const FPalCommand& Command)
{
	if (!AttackComponent || Command.CommandKind != EPalCommandKind::Attack || !Command.pTarget.IsValid() || bStartedAttacking)
	{
		return false;
	}
	UE_LOG(LogTemp, Log, TEXT("Executor_Attack :: StartCommand"));
	if (AttackComponent->GetTargetActor())
	{
		bStartedAttacking = true;
		IsCommandStarted = true;
		return true;
	}
	EndAttack();
	return false;
}

void UPalCommandExecutor_MonsterAttack::Abort()
{
	if (bStartedAttacking == false)
		return;
	bStartedAttacking = false;
	if (OwnerController)
	{
		OwnerController->ResetMove();
	}
}

void UPalCommandExecutor_MonsterAttack::WorkCommand()
{
	UE_LOG(LogTemp, Log, TEXT("Executor_Attack :: WorkCommand"));
	const FPalCommand& Command = *OwnerCommandComp->GetCurrentCommand_C();
	bool IsInRange = AttackComponent->TargetIsInRange();
	bStartedAttacking = true;
	IsCommandStarted = true;
	if (AttackComponent->IsSetTarget() || !IsInRange)
	{
		OwnerController->SetBBTargetActor(Command.pTarget.Get());
	}
}

bool UPalCommandExecutor_MonsterAttack::CheckCommandValid()
{
	if (!AttackComponent->GetTargetActor())
	{
		UE_LOG(LogTemp, Log, TEXT("Executor_Attack :: invalid target"));
		return false;
	}
	if (IAttackInterface::Execute_IsDead(AttackComponent->GetTargetActor()))
	{
		return false;
	}
	return true;
}

void UPalCommandExecutor_MonsterAttack::EndAttack()
{
	if (!bStartedAttacking)
		return;

	//const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
	//if (Command->pTarget.IsValid())
	//{
	//	bStartedAttacking = true;
	//	ESubAttackType AttackType = ESubAttackType::Default;
	//	//AttackType = (ESubAttackType)(FMath::Rand() % (uint8)ESubAttackType::Max_AttackType);
	//	AttackComponent->SetAttackData(AttackType);
	//	AttackComponent->SetAttackTarget(Command->pTarget.Get());
	//	OwnerController->SetBBTargetActor(Command->pTarget.Get());
	//	IsCommandStarted = true;
	//	return;
	//}
	Abort();
	EndCommand();
}