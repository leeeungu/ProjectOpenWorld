#include "Pal/Component/PalCommanderComponent.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Interface/CommanderManageable.h"
#include "Pal/Component/PalCommandComponent.h"

UPalCommanderComponent::UPalCommanderComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPalCommanderComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPalCommanderComponent::FinishCommand(AActor* PalActor, FPalCommand Command)
{
	ABaseCreature* pal = Cast< ABaseCreature>(PalActor);
	if (!pal || !pals.Contains(pal))
		return;
	NotWorkPals.push_back(pal);
	if (WorkList.size() > 0)
	{
		AActor* Work = WorkList.front();
		if (Work)
		{
			UE_LOG(LogTemp, Warning, TEXT("Finish and StartCommand %s"), *Work->GetFName().ToString());
			WorkList.pop_front();
			if (CommadReady(ICommanderManageable::Execute_GetCommand(Work)))
			{
			}
		}
	}
}

bool UPalCommanderComponent::StartWork(ABaseCreature* pal, const FPalCommand& Command)
{
	bool Result{};
	if (pal && pals.Contains(pal) && !pal->GetCommandComponent()->IsValidCommand())
	{
		Result = true;
	}
	return Result ;
}

void UPalCommanderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPalCommanderComponent::StorePal(AActor* NewPal)
{
	ABaseCreature* Creature = Cast< ABaseCreature>(NewPal);
	if (!Creature)
		return;
	pals.Add(Creature);
	NotWorkPals.push_back(Creature);
	Creature->GetCommandComponent()->OnCommandFinished.AddUniqueDynamic(this, &UPalCommanderComponent::FinishCommand);
}

void UPalCommanderComponent::RemovePal(AActor* targetPal)
{
	ABaseCreature* Creature = Cast< ABaseCreature>(targetPal);
	if (!Creature)
		return;
	Creature->GetCommandComponent()->OnCommandFinished.RemoveDynamic(this, &UPalCommanderComponent::FinishCommand);
	pals.Remove(Creature);
}

void UPalCommanderComponent::RegisterWork(AActor* WorkActor)
{
	if (!WorkActor)
		return;
	WorkList.push_back(WorkActor);
	if (WorkList.size() > 0)
	{
		AActor* Work = WorkList.front();
		if (Work)
		{
			UE_LOG(LogTemp, Warning, TEXT("Register and StartCommand %s"), *Work->GetFName().ToString());
			WorkList.pop_front();
			if (CommadReady(ICommanderManageable::Execute_GetCommand(Work)))
			{
			}
		}
	}
}

bool  UPalCommanderComponent::CommadAll(const FPalCommand& Command)
{
	if (Command.CommandKind == EPalCommandKind::None_PalCommandKind || Command.SubCommandType == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPalCommanderComponent :: UnDefined CommandWork"));
		return true;
	}
	TObjectPtr<ABaseCreature> pal{};
	while (!NotWorkPals.empty())
	{
		pal = NotWorkPals.front();
		NotWorkPals.pop_front();
		pal->GetCommandComponent()->ResetCommandQue();
		if (StartWork(pal, Command))
		{
			IPalCommandInterface::Execute_ReceiveCommand(pal, Command);
		}
	}
	return true;
}

bool UPalCommanderComponent::CommadReady(const FPalCommand& Command)
{
	if (Command.CommandKind == EPalCommandKind::None_PalCommandKind || Command.SubCommandType == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPalCommanderComponent :: UnDefined CommandWork"));
		return true;
	}
	bool Result{};
	//TObjectPtr<ABaseCreature> pal {};
	//int size = NotWorkPals.size();
	//while (!NotWorkPals.empty() && size > 0)
	//{
	//	pal = NotWorkPals.front();
	//	NotWorkPals.pop_front();
	//	//if (StartWork(pal, Command))
	//	//{
	//		UE_LOG(LogTemp, Warning, TEXT("StartWork %s %s"), *pal->GetName(), *Command.pTarget->GetName());
	//		IPalCommandInterface::Execute_ReceiveCommand(pal, Command);
	//		Result = true;
	//	//}
	//	size--;
	//}
	int size = NotWorkPals.size();
	for(TObjectPtr<ABaseCreature>& pal  : pals)
	{
		//pal = NotWorkPals.front();
		//NotWorkPals.pop_front();
		//if (StartWork(pal, Command))
		//{
			UE_LOG(LogTemp, Warning, TEXT("StartWork %s %s"), *pal->GetName(), *Command.pTarget->GetName());
			IPalCommandInterface::Execute_ReceiveCommand(pal, Command);
			Result = true;
		//}
		size--;
	}
	return Result;
}
