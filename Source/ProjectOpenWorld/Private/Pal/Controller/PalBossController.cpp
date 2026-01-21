#include "Pal/Controller/PalBossController.h"

APalBossController::APalBossController() : Super()
{
	//Script/AIModule.BehaviorTree'/Game/Pal/Model/Global/AI/Boss/BT_PalBoss.BT_PalBoss'
	ConstructorHelpers::FObjectFinder<UBehaviorTree> BT(TEXT("/Game/Pal/Model/Global/AI/Boss/BT_PalBoss.BT_PalBoss"));
	if (BT.Succeeded())
	{
		BTree = BT.Object;
	}
}
