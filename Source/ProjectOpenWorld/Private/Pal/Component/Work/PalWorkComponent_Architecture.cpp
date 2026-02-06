#include "Pal/Component/Work/PalWorkComponent_Architecture.h"
#include "Building/BaseBuilding.h"
#include "Building/Component/BuildingProgress.h"

UPalWorkComponent_Architecture::UPalWorkComponent_Architecture() : Super()
{
}

bool UPalWorkComponent_Architecture::CanWork()
{
	return Super::CanWork() && !TargetBuilding.IsValid();
}

void UPalWorkComponent_Architecture::WorkStart(const FPalCommand& Command)
{
	bIsArrive = true;
	bIsWorkEnd = false;
	TargetBuilding = MakeWeakObjectPtr(Cast< ABaseBuilding>(Command.pTarget));
	if (OwnerController && TargetBuilding.IsValid())
	{
		FVector Target = TargetBuilding->GetBuildingMeshComponent()->GetSocketLocation(TEXT("Bottom"));
		OwnerController->SetBBTargetLocation(Target);
	}
}

void UPalWorkComponent_Architecture::WorkEvent(const FPalCommand& Command)
{
	if (bIsWorking)
		return;
	bIsArrive = false;
	TargetBuilding = MakeWeakObjectPtr(Cast< ABaseBuilding>(Command.pTarget));
	if (TargetBuilding.IsValid())
	{
		bIsWorking = true;
		TargetBuilding->GetBuildingProgress()->StartBuilding(OwnerPal);
	}
}

void UPalWorkComponent_Architecture::WorkEnd(const FPalCommand& Command)
{
	WorkCancel();
}

void UPalWorkComponent_Architecture::WorkCancel()
{
	if (!bIsWorking)
		return;
	bIsWorking = false;
	bIsWorkEnd = true;
	if (TargetBuilding.IsValid())
	{
		TargetBuilding->GetBuildingProgress()->StopBuilding(OwnerPal);
		TargetBuilding = nullptr;
	}
}
