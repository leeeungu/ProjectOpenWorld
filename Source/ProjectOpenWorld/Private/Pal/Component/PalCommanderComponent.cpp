#include "Pal/Component/PalCommanderComponent.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Component/PalCommandComponent.h"

UPalCommanderComponent::UPalCommanderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPalCommanderComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPalCommanderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPalCommanderComponent::StorePal(AActor* NewPal)
{
	pals.Add(Cast< ABaseCreature>(NewPal));
}

void UPalCommanderComponent::RemovePal(AActor* targetPal)
{
	pals.Remove(Cast< ABaseCreature>(targetPal));
}

void UPalCommanderComponent::CommadAll(const FPalCommand& Command)
{
	for (ABaseCreature* pal : pals)
	{
		pal->GetCommandComponent()->ResetCommandQue();
		IPalCommandInterface::Execute_ReceiveCommand(pal, Command);
	}
}

void UPalCommanderComponent::CommadReady(const FPalCommand& Command)
{
	for (ABaseCreature* pal : pals)
	{
		if (!pal->GetCommandComponent()->IsValidCommand())
		{
			IPalCommandInterface::Execute_ReceiveCommand(pal, Command);
			return;
		}
	}
}
