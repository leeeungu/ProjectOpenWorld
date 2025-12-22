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
	if (!pal || !pals.Contains(pal) || pal->GetCommandComponent()->IsValidCommand())
		return;
	//pals.Find(pal);// ->Reset();
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
	if (WorkList.size() > 0)
	{
		AActor* Work = WorkList.front();
		WorkList.pop_front();
		if (Work)
		{
			if (ICommanderManageable::Execute_IsCommandFinished(Work))
			{
				return;
			}

			//UE_LOG(LogTemp, Warning, TEXT("UPalCommanderComponent::CommanderWork %s"), *Work->GetFName().ToString());
			if (CommadReady(ICommanderManageable::Execute_GetCommand(Work)) )
			{
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
		int PalSize = pals.Num();
		NotWork = 0;
		for (int i = 0; i < PalSize; i++)
		{
			if (StartWork(pals.Array()[i], {}))
			{
				NotWork++;
			}
		}

		int Size = WorkList.size();
		UE_LOG(LogTemp, Warning, TEXT(" UPalCommanderComponent::TickComponent %d %d"), WorkList.size(), NotWork);
		while (Size > 0)
		{
			CommanderWork();
			Size--;
		}
	}
}

void UPalCommanderComponent::StorePal(AActor* NewPal)
{
	ABaseCreature* Creature = Cast< ABaseCreature>(NewPal);
	if (!Creature)
		return;
	pals.Add(Creature, {});
	UE_LOG(LogTemp, Warning, TEXT(" UPalCommanderComponent::StorePal"));
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
	//if (ICommanderManageable::Execute_GetCommandKind(WorkActor) == EPalCommandKind::Work)
	//	return;
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
	bool Result{};
	//int Size = NotWorkPals.Num();
	//TArray<TObjectPtr<ABaseCreature>> Array = NotWorkPals.Array();
	/*for (int i = 0; i < Size && !Result; i++)
	{
		TObjectPtr<ABaseCreature> pal = *palIter;
		if (StartWork(pal, Command))
		{
			UE_LOG(LogTemp, Warning, TEXT("UPalCommanderComponent :: StartWork %s %s"), *pal->GetName(), *AActor::GetDebugName(Command.pTarget));
			palIter = NotWorkPals.erase(palIter);
			IPalCommandInterface::Execute_ReceiveCommand(pal, Command);
			Result = true;
		}
		else
		palIter++;
	}*/
	return Result;
}


bool UPalCommanderComponent::CommadReady(const FPalCommand& Command)
{
	if (Command.CommandKind == EPalCommandKind::None_PalCommandKind || Command.SubCommandType == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPalCommanderComponent :: UnDefined CommandWork"));
		return true;
	}
	bool Result{};

	int Size = pals.Num();
	TArray<TObjectPtr<ABaseCreature>> array = pals.Array();
	for (int i = 0; i < Size && !Result; i ++ )
	{
		TObjectPtr<ABaseCreature> pal = array[i];
		if (StartWork(pal, Command))
		{
			UE_LOG(LogTemp, Warning, TEXT("UPalCommanderComponent :: StartWork %s %s"), *pal->GetName(), *AActor::GetDebugName(Command.pTarget));
			Result = IPalCommandInterface::Execute_ReceiveCommand(pal, Command);
			//	pals[pal].Reset();
			break;
		}
	}
	return Result;
}
