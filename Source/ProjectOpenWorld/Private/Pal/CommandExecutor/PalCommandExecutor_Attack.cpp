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
		OwnerController->SetBBTargetActor(Command.pTarget.Get());
		IsCommandStarted = true;
		return true;
	}
	EndAttack();
	return false;
}

void UPalCommandExecutor_Attack::Abort()
{
	if (bStartedAttacking == false)
		return;
	bStartedAttacking = false;
	IsCommandStarted = false;
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
	
	const FPalCommand& Command = *OwnerCommandComp->GetCurrentCommand_C();
	UE_LOG(LogTemp, Warning, TEXT("Executor_Attack :: WorkCommand IsInRange %s "), *OwnerPal->GetName());
	if (!Command.pTarget.IsValid() || Command.CommandKind != EPalCommandKind::Attack || Command.pTarget != AttackComponent->GetTargetActor())
	{
		EndAttack();
	}
	if (AttackComponent->IsAttacking())
		return;
	bool IsInRange = AttackComponent->TargetIsInRange();
	if (!AttackComponent->IsSetTarget() || IsInRange == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor_Attack :: WorkCommand Set Target %s "), *OwnerPal->GetName());
		AttackComponent->SetAttackTarget(Command.pTarget.Get());
		OwnerController->ResetMove();
		OwnerController->SetBBTargetActor(Command.pTarget.Get());
	}
	else if (!AttackComponent->IsSetAttackData())
	{
		ESubAttackType AttackType = ESubAttackType::Default;
		//AttackType = (ESubAttackType)(FMath::Rand() % (uint8)ESubAttackType::Max_AttackType);
		AttackComponent->SetAttackData(AttackType);
	}
	else 
	{
		AttackComponent->StartAttack();
	}
}

bool UPalCommandExecutor_Attack::CheckCommandValid()
{
	if (!AttackComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor_Attack :: not self command or no target"));
		return false;
	}
	const FPalCommand& Command = *OwnerCommandComp->GetCurrentCommand_C();
	if (!Command.pTarget.IsValid() || Command.CommandKind != EPalCommandKind::Attack)
	{
		UE_LOG(LogTemp, Log, TEXT("Executor_Attack :: invalid target"));
		return false;
	}
	if (Command.pTarget->Implements<UAttackInterface>() && IAttackInterface::Execute_IsDead(Command.pTarget.Get()))
	{
		return false;
	}
	return true;
}

void UPalCommandExecutor_Attack::EndAttack()
{
	if (!bStartedAttacking)
		return;

	const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
	if (Command->pTarget.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor_Attack :: EndAttack Restart Attack %s "), *OwnerPal->GetName());
		bStartedAttacking = true;
		OwnerPal->SetActionStarted(true);
		ESubAttackType AttackType = ESubAttackType::Default;
		//AttackType = (ESubAttackType)(FMath::Rand() % (uint8)ESubAttackType::Max_AttackType);
		AttackComponent->SetAttackData(AttackType);
		AttackComponent->SetAttackTarget(Command->pTarget.Get());
		OwnerController->ResetMove();
		OwnerController->SetBBTargetActor(Command->pTarget.Get());
		IsCommandStarted = true;
		return;
	}
	Abort();
	EndCommand();
}

