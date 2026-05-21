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

void UPalMonsterCombatComponent::ReceiveDamage(const FPalDamagePayload& DamagePayload)
{
	if (DamagePayload.Instigator)
	{
		if (MonsterController)
			MonsterController->SetTargetActor(DamagePayload.Instigator);
		if (AIStateComponent)
			AIStateComponent->SetState(EPalAIMonsterState::Combat);
	}
}