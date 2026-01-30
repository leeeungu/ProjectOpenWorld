#include "Pal/Component/Work/PalWorkComponent_Attack.h"
#include "Pal/Component/PalAttackComponent.h"

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
	if(AttackComponent)
		AttackComponent->SetAttackData(ESubAttackType::Default);
	if (OwnerController)
		OwnerController->SetBBTargetActor(Command.pTarget.Get());
}

void UPalWorkComponent_Attack::WorkEvent(const FPalCommand& Command)
{
	if (bIsWorking && !AttackComponent && Command.pTarget.IsValid())
		return;
	bIsArrive = false;
	AttackTarget = Command.pTarget.Get();
	bIsWorking = true;
	AttackComponent->SetAttackTarget(Command.pTarget.Get());
	if (!AttackComponent->IsSetAttackData())
	{
		AttackComponent->SetAttackData(ESubAttackType::Default);
	}
	AttackComponent->StartAttack();
}

void UPalWorkComponent_Attack::WorkEnd(const FPalCommand& Command)
{
	WorkCancel();
}

void UPalWorkComponent_Attack::WorkCancel()
{
	UE_LOG(LogTemp, Log, TEXT("%s : Attack Work Cancel"), *GetOwner()->GetName());
	if (bIsWorking == false)
		return;
	AttackTarget = nullptr;
	bIsArrive = false;
	bIsWorking = false;
	if (AttackComponent)
	{
		//AttackComponent->SetAttackTarget
		AttackComponent->EndAttack();
		AttackComponent->ResetAttack();
	}
}
