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
	if (!pal || !pals.Contains(pal) || NotWorkPals.find(pal) != NotWorkPals.end())
		return;
	NotWorkPals.insert(pal);
	CommanderWork();
}

bool UPalCommanderComponent::StartWork(ABaseCreature* pal, const FPalCommand& Command)
{
	bool Result{};
	if (pal && pals.Contains(pal) && !pal->GetCommandComponent()->IsValidCommand() && pal->Implements<UPalCommandInterface>())
	{
		Result = true;
	}
	return Result ;
}

void UPalCommanderComponent::CommanderWork()
{
	if (WorkList.size() > 0 && NotWorkPals.size() > 0)
	{
		AActor* Work = WorkList.front();
		WorkList.pop_front();
		if (Work)
		{
			if (CommadReady(ICommanderManageable::Execute_GetCommand(Work)))
			{
				UE_LOG(LogTemp, Warning, TEXT("CommanderWork %s"), *Work->GetFName().ToString());
			}
			else
				WorkList.push_back(Work);
		}
	}
}

void UPalCommanderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Time += DeltaTime;
	if (Time >= 5.0f )
	{
		Time -= 5.0f;
		UE_LOG(LogTemp, Warning, TEXT(" UPalCommanderComponent::TickComponent %d, %d"), WorkList.size(), NotWorkPals.size());
		CommanderWork();
	}
}

void UPalCommanderComponent::StorePal(AActor* NewPal)
{
	ABaseCreature* Creature = Cast< ABaseCreature>(NewPal);
	if (!Creature)
		return;
	pals.Add(Creature);
	NotWorkPals.insert(Creature);
	Creature->GetCommandComponent()->OnCommandFinished.AddUniqueDynamic(this, &UPalCommanderComponent::FinishCommand);
	CommanderWork();
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
	if (!WorkActor || !WorkActor->Implements< UCommanderManageable>())
		return;

	WorkList.push_back(WorkActor);
	UE_LOG(LogTemp, Warning, TEXT("UPalCommanderComponent::Register %s"), *WorkActor->GetName());
	CommanderWork();
}

bool  UPalCommanderComponent::CommadAll(const FPalCommand& Command)
{
	if (Command.CommandKind == EPalCommandKind::None_PalCommandKind || Command.SubCommandType == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPalCommanderComponent :: UnDefined CommandWork"));
		return true;
	}
	TObjectPtr<ABaseCreature> pal{};
	/*while (!NotWorkPals.empty())
	{
		pal = NotWorkPals.front();
		pal->GetCommandComponent()->ResetCommandQue();
		if (StartWork(pal, Command))
		{
			NotWorkPals.pop_front();
			IPalCommandInterface::Execute_ReceiveCommand(pal, Command);
		}
	}*/
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

	int Size = NotWorkPals.size();
	std::set< TObjectPtr<ABaseCreature>>::iterator palIter = NotWorkPals.begin();
	for (int i = 0; i < Size && palIter != NotWorkPals.end(); i ++ )
	{
		TObjectPtr<ABaseCreature> pal = *palIter;
		if (StartWork(pal, Command))
		{
			UE_LOG(LogTemp, Warning, TEXT("UPalCommanderComponent :: StartWork %s %s"), *pal->GetName(), *AActor::GetDebugName(Command.pTarget));
			palIter = NotWorkPals.erase(palIter);
			IPalCommandInterface::Execute_ReceiveCommand(pal, Command);
			Result = true;
			break;
		}
		palIter++;
	}
	/*if (!NotWorkPals.empty())
	{
		TObjectPtr<ABaseCreature> pal = NotWorkPals.front();
		if (StartWork(pal, Command))
		{
			UE_LOG(LogTemp, Warning, TEXT("StartWork %s %s"), *pal->GetName(), *Command.pTarget->GetName());
			IPalCommandInterface::Execute_ReceiveCommand(pal, Command);
			Result = true;
			NotWorkPals.pop_front();
		}
	}*/
	return Result;
}
