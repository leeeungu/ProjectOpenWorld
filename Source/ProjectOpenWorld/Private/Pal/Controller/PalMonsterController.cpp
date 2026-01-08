#include "Pal/Controller/PalMonsterController.h"
#include "BehaviorTree/BehaviorTree.h" 

APalMonsterController::APalMonsterController()
{
	//Script/AIModule.BehaviorTree'/Game/Pal/Model/Global/AI/Monster/BT_PalMonster.BT_PalMonster'
	ConstructorHelpers::FObjectFinder<UBehaviorTree> BT(TEXT("/Game/Pal/Model/Global/AI/Monster/BT_PalMonster.BT_PalMonster"));
	if (BT.Succeeded())
	{
		MonsterBTree = BT.Object;
	}
}

void APalMonsterController::BeginPlay()
{
	APalAIController::BeginPlay();
	if (MonsterBTree)
		RunBehaviorTree(MonsterBTree);
}
