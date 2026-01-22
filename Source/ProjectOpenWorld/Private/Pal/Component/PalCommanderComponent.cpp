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
	for (TWeakObjectPtr<AActor> WorkActor : RegisteredWorks)
	{
		if (WorkActor.IsValid() && WorkActor->Implements<UCommanderManageable>())
		{
			if (!ICommanderManageable::Execute_IsCommandFinished(WorkActor.Get()))
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
	if (TargetWorkActor.IsValid() && TargetWorkActor.Get())
	{
		if (ICommanderManageable::Execute_IsCommandFinished(TargetWorkActor.Get()))
		{
			WorkQueue.Dequeue(TargetWorkActor);
			TargetWorkActor = nullptr;
		}
		else
		{
		
			if (WorkOnePal(ICommanderManageable::Execute_GetCommand(TargetWorkActor.Get())) && RegisteredWorks.Find(TargetWorkActor))
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
				//UE_LOG(LogTemp, Warning, TEXT("%s : Pal Commander Cannot Work On %s , ReEnqueue"), *GetOwner()->GetName(), *TargetWorkActor->GetName());
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
	{
		UE_LOG(LogTemp, Warning, TEXT("%s : Cannot Register Work , Not Implement ICommanderManageable"), *GetOwner()->GetName());
		return;
	}
	if (!ICommanderManageable::Execute_IsCommandFinished(WorkActor))
	{
		WorkQueue.Enqueue(WorkActor);
		QueSize++;
	}
	if (TargetWorkActor == WorkActor)
		TargetWorkActor = nullptr;
	RegisteredWorks.Add(WorkActor);
	WorkQueue.Empty();
	UpdateWorkQueue();
}

void UPalCommanderComponent::UnRegisterWork(AActor* WorkActor)
{
	UE_LOG(LogTemp, Warning, TEXT("%s : UnRegister Work %s"), *GetOwner()->GetName(), *WorkActor->GetName());
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
	ABaseCreature* pal = ArrayIter->Get();
	ArrayIter++;
	if (ArrayIter == pals.end())
		ArrayIter = pals.begin();
	
	return IPalCommandInterface::Execute_ReceiveCommand(pal, Command);
}
