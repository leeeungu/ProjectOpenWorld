#include "Pal/Controller/PalAIlyController.h"

APalAIlyController::APalAIlyController() : Super()
{
	//Script/AIModule.BehaviorTree'/Game/Pal/Model/Global/AI/PalAlly/BT_PalAlly.BT_PalAlly'
	ConstructorHelpers::FObjectFinder<UBehaviorTree> BT(TEXT("/Game/Pal/Model/Global/AI/PalAlly/BT_PalAlly.BT_PalAlly"));
	if (BT.Succeeded())
	{
		BTree = BT.Object;
	}
}
