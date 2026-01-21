#include "Pal/CommandExecutor/PalCommandExecutor_Attack.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Controller/PalAIController.h"
#include "Pal/Component/PalCommandComponent.h"

void UPalCommandExecutor_Attack::Initialize(UPalCommandComponent* CommandComp)
{
	UPalCommandExecutorBase::Initialize(CommandComp);
	if (OwnerCommandComp == nullptr)
		return;
	OwnerPal = Cast<ABaseCreature>(OwnerCommandComp->GetOwner());
	if (OwnerPal)
	{
		AttackComponent = OwnerPal->GetAttackComponent();
		OwnerController = Cast<APalAIController>(OwnerPal->GetController());
	}
	if (AttackComponent)
	{
		AttackComponent->OnPalAttackEnd.AddUniqueDynamic(this, &UPalCommandExecutor_Attack::EndAttack);
	}
}


bool UPalCommandExecutor_Attack::StartCommand(const FPalCommand& Command)
{
	if (!AttackComponent || Command.CommandKind != EPalCommandKind::Attack || !Command.pTarget.IsValid() || bStartedAttacking)
	{
		return false;
	}
	if (AttackComponent && OwnerController)
	{
		bStartedAttacking = true;
		OwnerPal->SetActionStarted(true);
		ESubAttackType AttackType = ESubAttackType::Default;
		//AttackType = (ESubAttackType)(FMath::Rand() % (uint8)ESubAttackType::Max_AttackType);
		AttackComponent->SetAttackData(AttackType);
		AttackComponent->SetAttackTarget(Command.pTarget.Get());
		OwnerController->SetBBTargetLocation(Command.pTarget->GetActorLocation());
		IsCommandStarted = true;
		return true;
	}
	EndAttack();
	return false;
}

void UPalCommandExecutor_Attack::Abort()
{
	UE_LOG(LogTemp, Log, TEXT("Executor_Attack :: Attack Abort"));
	if (bStartedAttacking == false)
		return;
	bStartedAttacking = false;
	if (OwnerPal)
	{
		OwnerPal->SetActionStarted(false);
		if (OwnerPal->GetController())
		{
			OwnerPal->GetController()->StopMovement();
		}
	}
	if (AttackComponent)
	{
		AttackComponent->EndAttack();
	}
	if (OwnerController)
	{
		OwnerController->ResetMove();
	}
}

void UPalCommandExecutor_Attack::WorkCommand()
{
	const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
	UE_LOG(LogTemp, Log, TEXT("Executor_Attack :: Attack WorkCommand"));
	if (!AttackComponent->TargetIsInRange())
	{
		OwnerController->SetBBTargetLocation(Command->pTarget->GetActorLocation());
		AttackComponent->SetAttackTarget(Command->pTarget.Get());
		ESubAttackType AttackType = ESubAttackType::Default;
		AttackComponent->SetAttackData(AttackType);
	}
	else if (!AttackComponent->IsAttacking())
	{
		OwnerController->ResetMove();
		AttackComponent->StartAttack();
	}
	if (!AttackComponent->IsSetTarget())
	{
		AttackComponent->SetAttackTarget(Command->pTarget.Get());
	}
	//UE_LOG(LogTemp, Log, TEXT("%s : Attack WorkCommand"), *OwnerPal->GetName());
}

bool UPalCommandExecutor_Attack::CheckCommandValid()
{
	if (!AttackComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor_Attack :: not self command or no target"));
		return false;
	}
	const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
	if (!Command->pTarget.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor_Attack :: invalid target"));
		return false;
	}
	return true;
}

void UPalCommandExecutor_Attack::EndAttack()
{
	Abort();
	EndCommand();
}

