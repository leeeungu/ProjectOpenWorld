#include "Pal/Controller/PalMonsterController.h"
#include "Pal/Component/PalAIStateComponent.h"

APalMonsterController::APalMonsterController()
{
	//Script/AIModule.BehaviorTree'/Game/Pal/Model/Global/AI/Monster/BT_PalMonster.BT_PalMonster'
	ConstructorHelpers::FObjectFinder<UBehaviorTree> BT(TEXT("/Game/Pal/Model/Global/AI/Monster/BT_PalMonster_v2.BT_PalMonster_v2"));
	if (BT.Succeeded())
	{
		BTree = BT.Object;
	}

	AIStateComponent = CreateDefaultSubobject<UPalAIStateComponent>(TEXT("AIStateComponent"));
}