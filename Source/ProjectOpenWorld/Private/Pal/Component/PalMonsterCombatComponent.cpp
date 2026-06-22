#include "Pal/Component/PalMonsterCombatComponent.h"
#include "GameFramework/Pawn.h"
#include "Pal/Controller/PalMonsterController.h"
#include "Pal/Data/PalDamageType.h"
#include "Pal/Component/PalAIStateComponent.h"

void UPalMonsterCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (APawn* OwnerMonster = Cast<APawn>(GetOwner()))
	{
		MonsterController = Cast<APalMonsterController>(OwnerMonster->GetController());
		if (MonsterController)
		{
			AIStateComponent = MonsterController->GetAIStateComponent();
		}
	}
}

void UPalMonsterCombatComponent::OnReceiveDamage(const FPalDamagePayload& DamagePayload)
{
	if (DamagePayload.Instigator)
	{
		SetCombatTarget(DamagePayload.Instigator);
	}
}

void UPalMonsterCombatComponent::ResetTarget()
{
	if (MonsterController)
		MonsterController->SetTargetActor(nullptr);
	if (AIStateComponent)
		AIStateComponent->SetState(EPalAIMonsterState::None);
}

void UPalMonsterCombatComponent::SetCombatTarget(AActor* Target)
{
	if (MonsterController)
		MonsterController->SetTargetActor(Target);
	if (AIStateComponent)
		AIStateComponent->SetState(EPalAIMonsterState::Combat);
}
