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
	bIsArrive = true;
	bIsWorkEnd = false;
	if (AttackComponent)
	{
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
			AttackComponent->StartAttack();
			OwnerController->ResetMove();
			return;
		}
	}
	if (OwnerController)
	{
		OwnerController->SetBBTargetActor(TargetCharacter.Get());
	}
}

void UPalWorkComponent_Attack::WorkEvent(const FPalCommand& Command)
{
	if (!AttackComponent || !Command.pTarget.IsValid() || AttackComponent->IsAttacking())
		return;
	if (AttackComponent->TargetIsInRange())
	{
		bIsWorking = true;
		AttackComponent->StartAttack();
		OwnerController->ResetMove();
	}
	else
	{
		WorkStart(Command);
	}
}

void UPalWorkComponent_Attack::WorkEnd(const FPalCommand& Command)
{
	WorkCancel();
}

void UPalWorkComponent_Attack::WorkCancel()
{
	if (AttackComponent && OwnerController)
	{
		if (AttackComponent)
		{
			AttackComponent->SetAttackTarget(TargetCharacter.Get());
			AttackComponent->SetAttackData(CurrentSubAttackType);

		}
		OwnerController->SetBBTargetActor(TargetCharacter.Get());
		return;
	}
	
	if (bIsWorking == false)
		return;
	bIsWorkEnd = true;
	AttackTarget = nullptr;
	bIsArrive = false;
	bIsWorking = false;
	if (AttackComponent)
	{
		//AttackComponent->SetAttackTarget
		AttackComponent->EndAttack();
	}
}
