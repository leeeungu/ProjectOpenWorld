#include "Creature/Character/BaseCreature.h"
#include "Creature/Component/CreatureAction_Building.h"
#include "Creature/Component/CreatureAttackComponent.h"

void ABaseCreature::BeginPlay()
{
	ABaseCharacter::BeginPlay();
	TSet<UActorComponent*> Coms =  GetComponents();
	AttackComponent =  Cast< UCreatureAttackComponent>(GetComponentByClass<UCreatureAttackComponent>());
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
		Type = ECreatureActionType::Action_None;
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
	if (ActionType != Type)
	{

		if (ActionComponents[(uint8)ActionType].GetObject() &&
			ActionComponents[(uint8)ActionType].GetObject())
		{
			ICreatureActionInterface::Execute_ActionEnd(ActionComponents[(uint8)ActionType].GetObject());
		}
		ActionType = Type;
		if (ActionComponents[(uint8)ActionType].GetObject() &&
			ActionComponents[(uint8)ActionType].GetObject())
		{
			if (bStart)
				ICreatureActionInterface::Execute_ActionStart(ActionComponents[(uint8)ActionType].GetObject(), ActionType, TargetObject);
			else
				ICreatureActionInterface::Execute_ActionEnd(ActionComponents[(uint8)ActionType].GetObject());
		}
	}
}

UCreatureAttackComponent* ABaseCreature::GetAttackComponent_Implementation() const
{
	return AttackComponent.Get();
}

float ABaseCreature::GetAttackDamage_Implementation() const
{
	if (AttackComponent)
	{
		return AttackComponent.Get()->GetDamage();
	}
	return 0.0f;
}
