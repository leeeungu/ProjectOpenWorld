#include "Pal/Controller/PalAIlyController.h"

APalAIlyController::APalAIlyController() : Super()
{
	//Script/AIModule.BehaviorTree'/Game/Pal/Model/Global/AI/PalAlly/BT_PalAlly_V2.BT_PalAlly_V2'
	//Script/AIModule.BehaviorTree'/Game/Pal/Model/Global/AI/PalAlly/BT_PalAlly_V2.BT_PalAlly_V2'
	ConstructorHelpers::FObjectFinder<UBehaviorTree> BT(TEXT("/Game/Pal/Model/Global/AI/PalAlly/BT_PalAlly_V2.BT_PalAlly_V2"));
	if (BT.Succeeded())
	{
		BTree = BT.Object;
	}
}
