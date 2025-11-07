#include "Creature/Character/BaseCreature.h"
#include "Creature/Component/CreatureAction_Building.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void ABaseCreature::BeginPlay()
{
	ABaseCharacter::BeginPlay();
	TSet<UActorComponent*> Coms = GetComponents();
	for (auto Component : Coms)
	{
		UCreatureActionComponent* Action = Cast< UCreatureActionComponent>(Component);
		if (Action)
		{
			ActionComponents[(uint8)Action->GetCreatureAction()] = TScriptInterface<ICreatureActionInterface>(Component);
		}
	}

	AAIController* OwnerController = Cast<AAIController>(GetController());
	if (OwnerController)
	{
		OwnerController->ReceiveMoveCompleted.AddDynamic(this, &ABaseCreature::FinishActionMove);
	}
	ResetActionMode();
}

void ABaseCreature::FinishActionMove(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result != EPathFollowingResult::Type::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Move Failed"));
		return;
	}
	ActionType = NextActionType;
	NextActionType = ECreatureActionType::Action_None;
	UE_LOG(LogTemp, Warning, TEXT("Move End %d"), (uint8)ActionType);
	if (ActionComponents[(uint8)ActionType] && ActionComponents[(uint8)ActionType].GetObject())
	{
		if (ICreatureActionInterface::Execute_ActionStart(ActionComponents[(uint8)ActionType].GetObject(), ActionFrom.Get(), TargetActor.Get()) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Reset Move"));

			ResetAction();
		}
	}
}

bool ABaseCreature::MoveToTarget()
{
	AAIController* OwnerController = Cast<AAIController>(GetController());
	if (TargetActor)
	{
		FAIMoveRequest MoveReq(TargetActor.Get());
		MoveReq.SetUsePathfinding(true);
		MoveReq.SetAllowPartialPath(false);
		MoveReq.SetAcceptanceRadius(100.0f);
		MoveReq.SetReachTestIncludesAgentRadius(true);
		MoveReq.SetCanStrafe(true);
		if (OwnerController)
		{
			FNavPathSharedPtr OutPath{};
			OwnerController->MoveTo(MoveReq, &OutPath);
			if (!OutPath.IsValid())
				return false;
		}
	}
	return true;
}

void ABaseCreature::ReceiveMessage_Implementation(EMessageType MessageType, AActor* SendActor, AActor* TargetObject)
{
}

void ABaseCreature::ReceiveActionMessage_Implementation(ECreatureActionType MessageType, AActor* SendActor, AActor* TargetObject)
{
	AAIController* OwnerController = Cast<AAIController>(GetController());
	if (ActionComponents[(uint8)NextActionType] &&
		ActionComponents[(uint8)NextActionType].GetObject() )
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
	}
}


bool ABaseCreature::GetIsActionStarted(ECreatureActionType Type)
{
	if (ActionComponents[(uint8)Type] &&
		ActionComponents[(uint8)Type].GetObject())
	{
		return Cast< UCreatureActionComponent>(ActionComponents[(uint8)Type].GetObject())->GetIsStarted();
	}
	return false;
}

void ABaseCreature::ResetAction()
{
	ActionType = ECreatureActionType::Action_None;
	NextActionType = ECreatureActionType::Action_None;
	TargetActor = nullptr;
	ActionFrom = nullptr;
}

void ABaseCreature::ResetActionMode()
{
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

void ABaseCreature::TransportActionMode()
{
	GetCharacterMovement()->MaxWalkSpeed = 50.0f;
}

bool ABaseCreature::DamagedCharacter_Implementation(const TScriptInterface<IAttackInterface>& Other)
{
	return false;
}

float ABaseCreature::GetAttackValue_Implementation() const
{
	return Attack;
}

void ABaseCreature::SetAttackValue_Implementation(float NewValue)
{
	Attack = NewValue;
}

void ABaseCreature::RetAttackValue_Implementation()
{
	Attack = 1.0f;
}
