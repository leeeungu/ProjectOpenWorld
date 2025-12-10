#include "Pal/Component/PalCommanderComponent.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Interface/CommanderManageable.h"
#include "Pal/Component/PalCommandComponent.h"

UPalCommanderComponent::UPalCommanderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
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
	NotWorkPals.Enqueue(pal);
	if (!WorkList.empty())
	{
		AActor* Work = WorkList.front();
		if (CommadReady(ICommanderManageable::Execute_GetCommand(Work)))
		{
			WorkList.pop_front();
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
	NotWorkPals.Enqueue(Creature);
	Creature->GetCommandComponent()->OnCommandFinished.AddDynamic(this, &UPalCommanderComponent::FinishCommand);
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
	WorkList.push_back(WorkActor);
	if (!WorkList.empty())
	{
		AActor* Work = WorkList.front();
		if (CommadReady(ICommanderManageable::Execute_GetCommand(Work)))
		{
			WorkList.pop_front();
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
	ABaseCreature* pal{};
	while (!NotWorkPals.IsEmpty())
	{
		NotWorkPals.Dequeue(pal);
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
	ABaseCreature* pal {};
	while (!NotWorkPals.IsEmpty())
	{
		NotWorkPals.Dequeue(pal);
		if (StartWork(pal, Command))
		{
			IPalCommandInterface::Execute_ReceiveCommand(pal, Command);
			Result = true;
		}
	}
	return Result;
}
