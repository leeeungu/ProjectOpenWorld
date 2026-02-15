#include "Pal/Component/Work/PalWorkComponent_Attack.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Creature/Character/BaseMonster.h"
#include "Pal/Factory/PalCommandFunctionLibrary.h"

UPalWorkComponent_Attack::UPalWorkComponent_Attack() : Super()
{

}

void UPalWorkComponent_Attack::BeginPlay()
{
	Super::BeginPlay();
	if (OwnerPal)
	{
		AttackComponent = OwnerPal->GetAttackComponent();
	}
	else if (ABaseMonster* Monster = Cast<ABaseMonster>(GetOwner()))
	{
		AttackComponent = Monster->GetAttackComponent();
	}
	if (AttackComponent)
	{
		AttackComponent->OnPalAttackEnd.AddUniqueDynamic(this, &UPalWorkComponent_Attack::WorkCancel);
	}
}


bool UPalWorkComponent_Attack::CanWork()
{
	return Super::CanWork() && AttackComponent && (!AttackTarget.GetObject() || !IAttackInterface::Execute_IsDead(AttackTarget.GetObject()));
}

void UPalWorkComponent_Attack::WorkStart(const FPalCommand& Command)
{
	if (AttackComponent)
	{
		bIsArrive = true;
		bIsWorkEnd = false;
		CurrentSubAttackType = static_cast<ESubAttackType>(Command.SubCommandType);
		if (CurrentSubAttackType <= ESubAttackType::None_AttackType  || CurrentSubAttackType >= ESubAttackType::Max_AttackType)
		{
			CurrentSubAttackType = ESubAttackType::Default;
		}
		AttackComponent->SetAttackData(CurrentSubAttackType);
		TargetCharacter = Command.pTarget.Get();
		AttackComponent->SetAttackTarget(TargetCharacter.Get());
		if (AttackComponent->TargetIsInRange()) 
		{
			bIsWorking = true;
			OwnerController->ResetMove();
			AttackComponent->StartAttack();
			return;
		}
		if (OwnerController)
		{
			OwnerController->SetBBTargetActor(TargetCharacter.Get(), AttackComponent->GetAttackDistance());
		}
	}
}

void UPalWorkComponent_Attack::WorkEvent(const FPalCommand& Command)
{
	if (!AttackComponent || !TargetCharacter.IsValid())
		return;
	if (!IAttackInterface::Execute_IsDead(TargetCharacter.Get()))
	{
		//if (AttackComponent->TargetIsInRange()) // 왜 범위 검사 가 계속 정확히가 아닐까...
		if (!AttackComponent->IsAttacking())
		{
			bIsWorking = true;
			OwnerController->ResetMove();
			AttackComponent->StartAttack();
			return;
		}
		/*else if (OwnerController)
		{
			OwnerController->SetBBTargetActor(TargetCharacter.Get(), AttackComponent->GetAttackDistance());
		}*/
	}
	bIsWorkEnd = true;
	AttackTarget = nullptr;
	bIsArrive = false;
	if (AttackComponent && AttackComponent->IsAttacking())
	{
		AttackComponent->EndAttack();
	}
}

void UPalWorkComponent_Attack::WorkEnd(const FPalCommand& Command)
{
	if (bIsWorking == false)
		return;
	bIsWorkEnd = true;
	AttackTarget = nullptr;
	bIsArrive = false;
	bIsWorking = false;
	if (AttackComponent && AttackComponent->IsAttacking())
	{
		AttackComponent->EndAttack();
	}
}

void UPalWorkComponent_Attack::WorkCancel()
{
	if (bIsWorking == false )
		return;
	bIsWorking = false;
	if (AttackComponent && OwnerController && AttackComponent->IsTargetNotDead() && TargetCharacter.IsValid())
	{
		AttackComponent->SetAttackTarget(TargetCharacter.Get());
		AttackComponent->SetAttackData(CurrentSubAttackType);
		if (AttackComponent->TargetIsInRange())
		{
			OwnerController->ResetMove();
			bIsWorking = true;
			AttackComponent->StartAttack();
			return;
		}
		if (OwnerController)
			OwnerController->SetBBTargetActor(TargetCharacter.Get(), AttackComponent->GetAttackDistance());
		return;
	}

	bIsWorkEnd = true;
	AttackTarget = nullptr;
	bIsArrive = false;
	if (AttackComponent && AttackComponent->IsAttacking())
	{
		AttackComponent->EndAttack();
	}
}
