#include "Creature/Component/CreatureAttackComponent.h"
#include "GameFramework/Character.h"

UCreatureAttackComponent::UCreatureAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCreatureAttackComponent::SetAttackIndex(int montageIndex)
{
	MontageIndex = montageIndex;
}

void UCreatureAttackComponent::PlayAttackMontage(int montageIndex) const
{
	if (montageIndex == -1)
		montageIndex = MontageIndex;
	if (!OwnerCharacter || !OwnerCharacter->GetMesh() || !ArrayMontage.IsValidIndex(montageIndex) || !ArrayMontage[montageIndex].Montage)
		return;
	UAnimInstance*  AnimInst = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInst)
		return;
	AnimInst->Montage_Play(ArrayMontage[montageIndex].Montage.Get());
}

bool UCreatureAttackComponent::GetAttackData(int montageIndex, FCreatureAttackData& Data) const
{
	if(!ArrayMontage.IsValidIndex(montageIndex))
		return false;
	Data = ArrayMontage[montageIndex];
	return true;
}

float UCreatureAttackComponent::GetDamage() const
{
	if (!ArrayMontage.IsValidIndex(MontageIndex))
		return 0;
	FCreatureAttackData Data = ArrayMontage[MontageIndex];
	return Data.AttackDamage;
}

void UCreatureAttackComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ACharacter>(GetOwner());
}

