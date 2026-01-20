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

void UPalCommanderComponent::UpdateWorkQueue()
{
	if (!WorkQueue.IsEmpty())
		return;
	QueSize = 0;
	for (AActor* WorkActor : RegisteredWorks)
	{
		if (WorkActor && WorkActor->Implements<UCommanderManageable>())
		{
			if (!ICommanderManageable::Execute_IsCommandFinished(WorkActor))
			{
				WorkQueue.Enqueue(WorkActor);
				QueSize++;
			}
		}
	}
}

void UPalCommanderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (TargetWorkActor && TargetWorkActor.Get())
	{
		if (ICommanderManageable::Execute_IsCommandFinished(TargetWorkActor.Get()) || RegisteredWorks.Find(TargetWorkActor) == nullptr)
		{
			WorkQueue.Dequeue(TargetWorkActor);
			TargetWorkActor = nullptr;
		}
		else
		{
			if (WorkOnePal(ICommanderManageable::Execute_GetCommand(TargetWorkActor.Get())))
			{
				int NotWork = 0;
				for (const TObjectPtr<ABaseCreature>& Temp : pals)
				{
					if (Temp && !Temp->GetCommandComponent()->IsValidCommand())
					{
						NotWork++;
						break;
					}
				}
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
	}
	else
	{
		if (!WorkQueue.Peek(TargetWorkActor))
		{
			UpdateWorkQueue();
		}
	}
}

void UPalCommanderComponent::StopAllPal()
{
	for (const TObjectPtr<ABaseCreature>& Temp : pals)
	{
		if (Temp)
		{
			Temp->GetCommandComponent()->ResetCommandQue();
			//IPalCommandInterface::Execute_ReceiveCommand(Temp.Get(), FPalCommand{ EPalCommandKind::Stop });
		}
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
	if (!ICommanderManageable::Execute_IsCommandFinished(WorkActor))
	{
		WorkQueue.Enqueue(WorkActor);
		QueSize++;
	}
	RegisteredWorks.Add(WorkActor);
}

void UPalCommanderComponent::UnRegisterWork(AActor* WorkActor)
{
	RegisteredWorks.Remove(WorkActor);
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
	UE_LOG(LogTemp, Warning, TEXT(" UPalCommanderComponent::WorkOnePal %d"), Command.CommandKind);
	if (Command.pTarget.Get())
		UE_LOG(LogTemp, Warning, TEXT(" UPalCommanderComponent::Target %s"), *Command.pTarget.Get()->GetName());
	ABaseCreature* pal = ArrayIter->Get();
	ArrayIter++;
	if (ArrayIter == pals.end())
		ArrayIter = pals.begin();
	return IPalCommandInterface::Execute_ReceiveCommand(pal, Command);
}
