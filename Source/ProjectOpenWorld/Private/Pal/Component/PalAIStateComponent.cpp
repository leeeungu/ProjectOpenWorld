#include "Pal/Component/PalAIStateComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Pal/FunctionLibrary/PalAIBlackboardKeysLibrary.h"

void UPalAIStateComponent::SetBBValue(EPalAIMonsterState NewState)
{
	if (AAIController* OwnerAIController = Cast<AAIController>(GetOwner()))
	{
		if (UBlackboardComponent* BlackboardComp = OwnerAIController->GetBlackboardComponent())
		{
			BlackboardComp->SetValueAsEnum(UPalAIBlackboardKeysLibrary::GetBBMonsterStateKey(), static_cast<uint8>(NewState));
		}
	}
}

void UPalAIStateComponent::SetState(EPalAIMonsterState NewState)
{
	EPalAIMonsterState PreState = GetState();
	if (PreState == NewState)
	{
		return;
	}
	SetBBValue(NewState);
	OnStateChanged.Broadcast(PreState, NewState);
}

EPalAIMonsterState UPalAIStateComponent::GetState() const
{
	if (AAIController* OwnerAIController = Cast<AAIController>(GetOwner()))
	{
		if (UBlackboardComponent* BlackboardComp = OwnerAIController->GetBlackboardComponent())
		{
			uint8 StateValue = BlackboardComp->GetValueAsEnum(UPalAIBlackboardKeysLibrary::GetBBMonsterStateKey());
			return static_cast<EPalAIMonsterState>(StateValue);
		}
	}
	return EPalAIMonsterState::Idle;
}
