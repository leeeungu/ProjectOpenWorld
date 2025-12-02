#include "Pal/Component/PalAllyCommandComponent.h"

void UPalAllyCommandComponent::OnStartCurrentCommand()
{
	if (IsValidCommand() == false)
		return;
	FPalCommand* Current = GetCurrentCommand_C();
	Current->CommandKind;


	/*AAIController* OwnerController = Cast<AAIController>(GetController());
	if (ActionComponents[(uint8)NextActionType] &&
		ActionComponents[(uint8)NextActionType].GetObject())
	{
		UE_LOG(LogTemp, Warning, TEXT("NextActionType End"));
		ICreatureActionInterface::Execute_ActionEnd(ActionComponents[(uint8)NextActionType].GetObject());
	}
	if (ActionComponents[(uint8)ActionType] &&
		ActionComponents[(uint8)ActionType].GetObject())
	{
		UE_LOG(LogTemp, Warning, TEXT("ActionType End"));
		ICreatureActionInterface::Execute_ActionEnd(ActionComponents[(uint8)ActionType].GetObject());
	}

	UE_LOG(LogTemp, Warning, TEXT("ReceiveActionMessage %d"), (uint8)MessageType);
	ResetAction();
	ResetActionMode();
	OwnerController->StopMovement();
	if (MessageType != ECreatureActionType::Action_None)
	{
		UE_LOG(LogTemp, Warning, TEXT("Transport Move"));
		ActionType = ECreatureActionType::Action_Move;
		NextActionType = MessageType;
		TargetActor = TargetObject;
		ActionFrom = SendActor;
		if (!MoveToTarget())
		{
			ResetAction();
		}
	}
	else
	{
		for (int i = 1; i < (uint8)ECreatureActionType::Action_Max; i++)
		{
			if (ActionComponents[i] &&
				ActionComponents[i].GetObject())
			{
				ICreatureActionInterface::Execute_ActionEnd(ActionComponents[i].GetObject());
			}
		}
	}*/
}

void UPalAllyCommandComponent::OnEndCurrentCommand()
{
}
