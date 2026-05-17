#include "Pal/Controller/PalMonsterController.h"
#include "GameBase/BaseCharacter.h"
#include "Pal/Component/PalAIStateComponent.h"
#include "Pal/Data/PalDamageType.h"
#include "Pal/FunctionLibrary/PalAIBlackboardKeysLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameBase/Interface/AttackInterface.h"
#include "Pal/Component/PalHitHandlerComponent.h"

APalMonsterController::APalMonsterController() : Super{}
{
	//Script/AIModule.BehaviorTree'/Game/Pal/Model/Global/AI/Monster/BT_PalMonster_v2.BT_PalMonster_v2'
	ConstructorHelpers::FObjectFinder<UBehaviorTree> BT(TEXT("/Game/Pal/Model/Global/AI/Monster/BT_PalMonster_v2.BT_PalMonster_v2"));
	if (BT.Succeeded())
	{
		BTree = BT.Object;
	}

	AIStateComponent = CreateDefaultSubobject<UPalAIStateComponent>(TEXT("AIStateComponent"));
}

void APalMonsterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	//if (IAttackInterface* Attack = Cast< IAttackInterface>(OwnerPal))
	//{
	//	if (UPalHitHandlerComponent* HitHandler = Attack->GetHitHandlerComponent())
	//	{
	//		HitHandler->OnDamageTaken.AddUniqueDynamic(this, &APalMonsterController::RecieveDamage);
	//	}
	//}
}

void APalMonsterController::BeginPlay()
{
	Super::BeginPlay();

	if (AIStateComponent)
	{
		AIStateComponent->SetState(EPalAIMonsterState::Idle);
	}
}

//void APalMonsterController::RecieveDamage(const FPalDamagePayload& DamagePayload)
//{
//	if (DamagePayload.Instigator)
//	{
//		SetTargetActor(DamagePayload.Instigator);
//		if(AIStateComponent)
//			AIStateComponent->SetState(EPalAIMonsterState::Chase);
//	}
//}

void APalMonsterController::SetTargetActor(AActor* Target)
{
	if (GetBlackboardComponent())
	{
		GetBlackboardComponent()->SetValueAsObject(UPalAIBlackboardKeysLibrary::GetBBTargetActorKey(), Target);
	}
}