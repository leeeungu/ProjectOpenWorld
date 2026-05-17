#include "Pal/Component/PalAIStateComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Pal/FunctionLibrary/PalAIBlackboardKeysLibrary.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"

void UPalAIStateComponent::BeginPlay()
{
	if (AAIController* OwnerAIController = Cast<AAIController>(GetOwner()))
	{
		BlackboardComponent = OwnerAIController->GetBlackboardComponent();
	}
	Super::BeginPlay();
}

void UPalAIStateComponent::SetBBValue(EPalAIMonsterState NewState)
{
	if (!BlackboardComponent.IsValid())
	{
		if (AAIController* OwnerAIController = Cast<AAIController>(GetOwner()))
		{
			BlackboardComponent = OwnerAIController->GetBlackboardComponent();
		}
	}

	if (BlackboardComponent.IsValid())
	{
		BlackboardComponent->SetValueAsEnum(UPalAIBlackboardKeysLibrary::GetBBMonsterStateKey(), static_cast<uint8>(NewState));
	}
	CurrentState = NewState;
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
	if (BlackboardComponent.IsValid())
	{
		uint8 StateValue = BlackboardComponent->GetValueAsEnum(UPalAIBlackboardKeysLibrary::GetBBMonsterStateKey());
		if(StateValue == UBlackboardKeyType_Enum::InvalidValue)
			return EPalAIMonsterState::None;
		return static_cast<EPalAIMonsterState>(StateValue);
	}
	return EPalAIMonsterState::None;
}
