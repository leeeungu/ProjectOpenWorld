#include "Pal/Controller/PalAIlyController.h"
#include "Pal/FunctionLibrary/PalAIBlackboardKeysLibrary.h"
#include "Pal/Component/PalJobComponent.h"
#include "Pal/Data/PalJobTypes.h"
#include "Pal/Character/PalBaseCreature.h"

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

void APalAIlyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (APalBaseCreature* Creature = Cast< APalBaseCreature>(InPawn))
	{
		if (UPalJobComponent* JobComponent = Creature->GetPalJobComponent())
		{
			JobComponent->OnJobAssigned.AddUniqueDynamic(this, &APalAIlyController::OnJobAssigned);
			JobComponent->OnJobFinished.AddUniqueDynamic(this, &APalAIlyController::OnJobFinished);
			JobComponent->OnJobLocationChange.AddUniqueDynamic(this, &APalAIlyController::OnJobLocationChange);
			JobComponent->OnJobTargetChange.AddUniqueDynamic(this, &APalAIlyController::OnJobTargetChange);
		}
	}
}

void APalAIlyController::OnJobAssigned(const FPalWorkCommand& Job)
{
	SetBBEnum(UPalAIBlackboardKeysLibrary::GetBBJobType(), (uint8)Job.JobType);
	SetBBActor(UPalAIBlackboardKeysLibrary::GetBBJobTarget(), Job.pTarget.Get());
	SetBBLocation(UPalAIBlackboardKeysLibrary::GetBBJobLocation(), Job.TargetLocation);
}

void APalAIlyController::OnJobFinished(const FPalWorkCommand& Job, bool bSuccess)
{
	SetBBEnum(UPalAIBlackboardKeysLibrary::GetBBJobType(), static_cast<uint8>(EPalJobType::None));
	ClearBBValue(UPalAIBlackboardKeysLibrary::GetBBJobTarget());
	ClearBBValue(UPalAIBlackboardKeysLibrary::GetBBJobLocation());
}

void APalAIlyController::OnJobLocationChange(FVector NewLocation)
{
	SetBBLocation(UPalAIBlackboardKeysLibrary::GetBBJobLocation(), NewLocation);
}

void APalAIlyController::OnJobTargetChange(AActor* NewActor)
{
	SetBBActor(UPalAIBlackboardKeysLibrary::GetBBJobTarget(), NewActor);
}
