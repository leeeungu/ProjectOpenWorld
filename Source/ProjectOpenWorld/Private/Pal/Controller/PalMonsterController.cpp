#include "Pal/Controller/PalMonsterController.h"

APalMonsterController::APalMonsterController()
{
	//Script/AIModule.BehaviorTree'/Game/Pal/Model/Global/AI/Monster/BT_PalMonster.BT_PalMonster'
	ConstructorHelpers::FObjectFinder<UBehaviorTree> BT(TEXT("/Game/Pal/Model/Global/AI/Monster/BT_PalMonster.BT_PalMonster"));
	if (BT.Succeeded())
	{
		BTree = BT.Object;
	}
}