#include "Creature/Character/BaseCreature.h"
#include "Creature/Component/CreatureAction_Building.h"

void ABaseCreature::BeginPlay()
{
	ABaseCharacter::BeginPlay();
	TSet<UActorComponent*> Coms =  GetComponents();

	for (auto Component : Coms)
	{
		UCreatureActionComponent* Action = Cast< UCreatureActionComponent>(Component);
		if (Action)
		{
			ActionComponents[(uint8)Action->GetCreatureAction()] = TScriptInterface<ICreatureActionInterface>(Component);
		}
	}
}

void ABaseCreature::ReceiveMessage_Implementation(EMessageType MessageType, AActor* SendActor, UObject* TargetObject)
{
	ECreatureActionType Type{};
	bool bStart{};
	switch (MessageType)
	{
	case EMessageType::NONE:
		break;
	case EMessageType::TakeRest:
		break;
	case EMessageType::DoBuild:
	{
		bStart = true;
		Type = ECreatureActionType::Action_Buliding;
		break;
	}
	case EMessageType::StopBuild:
	{
		Type = ECreatureActionType::Action_Buliding;
		break;
	}
	case EMessageType::DoAttack:
	{
		Type = ECreatureActionType::Action_Attack;
		bStart = true;
		break;
	}
	case EMessageType::StopAttack:
	{
		Type = ECreatureActionType::Action_Attack;
		break;
	}
	case EMessageType::DoCraft:
		break;
	case EMessageType::StopCraft:
		break;
	default:
		break;
	}

	if (ActionComponents[(uint8)Type].GetObject() &&
		ActionComponents[(uint8)Type].GetObject())
	{
		if (bStart)
			ICreatureActionInterface::Execute_ActionStart(ActionComponents[(uint8)Type].GetObject(), Type, TargetObject);
		else
			ICreatureActionInterface::Execute_ActionEnd(ActionComponents[(uint8)Type].GetObject());
	}
}
