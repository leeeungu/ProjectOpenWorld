#include "Creature/Component/CreatureAction_Transport.h"
#include "Creature/Character/BaseCreature.h"

UCreatureAction_Transport::UCreatureAction_Transport()
{
	Action = ECreatureActionType::Action_Transport;
}

// v1
// 총 2번의 호출이 일어난다. 
// 첫 번째는 상자 or 펠 거점이 SendActor /  적재 대상이 TargetActor으로
// TargetActor으로 basecreature에서 이동이 이뤄나고 도착시 ActionStart 가상 함수에서 서로 바꿔서 다시 자신에게 ActionMessage를 호출한다.
// basecreature에 의해 다시 SendActor로 이동할 것이고 ActionStart에서 저장해둔 actor들 비교로 해당 2번째 전달인지 판단하여 적재인터페이스를 실행한다.
bool UCreatureAction_Transport::ActionStart_Implementation( AActor* SendActor, AActor* TargetActor)
{
	// v1
	if (bActionStart && TransportActor == SendActor && BoxActor == TargetActor)
	{
		// 적재 함수 호출 및 초기화
		UE_LOG(LogTemp, Warning, TEXT("Transport End"));
		bActionStart = false;
		TransportActor = nullptr;
		BoxActor = nullptr;
		if (ABaseCreature* pOwner = Cast< ABaseCreature>(GetOwner()))
		{
			pOwner->ResetAction();
		}
		return true;
	}
	if (bActionStart)
		return false;

	if (ABaseCreature* pOwner = Cast< ABaseCreature>(GetOwner()))
	{
		pOwner->ResetAction();
	}
	bActionStart = false;
	TransportActor = nullptr;
	BoxActor = nullptr;
	if (!bActionStart && !TransportActor && !BoxActor && SendActor && TargetActor)
	{
		// 적재 아이템 접근 및 이동 방향 반대로 변경
		bActionStart = true;
		TransportActor = TargetActor;
		BoxActor = SendActor;
		UE_LOG(LogTemp, Warning, TEXT("Transport Start"));
		ICreatureMessageInterface::Execute_ReceiveActionMessage(GetOwner(), Action, TransportActor.Get(), BoxActor.Get());
		if (ABaseCreature* pOwner = Cast< ABaseCreature>(GetOwner()))
		{
			pOwner->TransportActionMode();
		}
	}
	return true;
}

bool UCreatureAction_Transport::ActionEnd_Implementation()
{
	if (bActionStart == false)
		return false;
	if (bActionStart && TransportActor && BoxActor )
	{
		bActionStart = false;
		TransportActor = nullptr;
		BoxActor = nullptr;
	}
	return true;
}


// v2
// 상자 or 펠 거점이  아이템을 target으로 주고 적재할actor를 sendactor로 넣어 message 한다
// basecreature의 기본 구현에 의해 TargetActor 대상으로 갈 것이고
// 해당 가상 함수에 의해 다시 적재할 SendActor 위치로 message 할 것이다.
// 도착 시 SendActor의 인터페이스를 통해 적재한다. (도착시를 알기 위해 interface를 하나 더 두자)