#include "Pal/Component/PalCommanderComponent.h"
#include "Pal/Interface/CommanderManageable.h"
#include "Pal/Component/PalCommandComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

UPalCommanderComponent::UPalCommanderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPalCommanderComponent::BeginPlay()
{
	Super::BeginPlay();
	ArrayIter = pals.begin();
}

void UPalCommanderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (TargetWorkActor && TargetWorkActor.Get())
	{
		if (WorkOnePal(ICommanderManageable::Execute_GetCommand(TargetWorkActor.Get())))
		{
			int NotWork = 0;
			for (const TObjectPtr<ABaseCreature>& Temp : pals)
			{
				if (Temp && !Temp->GetCommandComponent()->IsValidCommand())
				{
					NotWork++;
				}
			}
			UE_LOG(LogTemp, Warning, TEXT(" UPalCommanderComponent::TickComponent %d %d"), QueSize, NotWork);
			WorkQueue.Dequeue(TargetWorkActor);
			TargetWorkActor = nullptr;
			QueSize--;
		}
		else
		{
			if (ArrayIter != pals.end())
			{
				WorkQueue.Dequeue(TargetWorkActor);
				WorkQueue.Enqueue(TargetWorkActor);
				TargetWorkActor = nullptr;
			}
		}
	}
	else
	{
		WorkQueue.Peek(TargetWorkActor);
	}
}

void UPalCommanderComponent::StorePal(AActor* NewPal)
{
	ABaseCreature* Creature = Cast< ABaseCreature>(NewPal);
	if (!Creature || !Creature->Implements<UPalCommandInterface>())
		return;
	pals.insert(Creature);
	ArrayIter = pals.begin();
}

void UPalCommanderComponent::RemovePal(AActor* targetPal)
{
	ABaseCreature* Creature = Cast< ABaseCreature>(targetPal);
	if (!Creature)
		return;
	pals.erase(Creature);
	ArrayIter = pals.begin();
}

void UPalCommanderComponent::RegisterWork(AActor* WorkActor)
{
	if (!WorkActor || !WorkActor->Implements< UCommanderManageable>())
		return;
	WorkQueue.Enqueue(WorkActor);
	QueSize++;
}

bool UPalCommanderComponent::WorkAllPal(const FPalCommand& Command)
{
	bool bWorked = false;
	for (const TObjectPtr<ABaseCreature>& Temp : pals)
	{
		if (Temp && IPalCommandInterface::Execute_ReceiveCommand(Temp.Get(), Command))
		{
			bWorked = true;
		}
	}
	return bWorked;
}

bool UPalCommanderComponent::WorkOnePal(const FPalCommand& Command)
{
	if (ArrayIter == pals.end())
		return false;
	ABaseCreature* pal = ArrayIter->Get();
	ArrayIter++;
	if (ArrayIter == pals.end())
		ArrayIter = pals.begin();
	return IPalCommandInterface::Execute_ReceiveCommand(pal, Command);
}
