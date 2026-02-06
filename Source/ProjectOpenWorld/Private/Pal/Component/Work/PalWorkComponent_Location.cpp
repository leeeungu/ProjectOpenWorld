#include "Pal/Component/Work/PalWorkComponent_Location.h"

UPalWorkComponent_Location::UPalWorkComponent_Location()
{
}

void UPalWorkComponent_Location::BeginPlay()
{
	Super::BeginPlay();
}

bool UPalWorkComponent_Location::CanWork()
{
	return Super::CanWork();
}

void UPalWorkComponent_Location::WorkStart(const FPalCommand& Command)
{
	if (bIsWorking)
		return;
	bIsWorking = true;
	bIsWorkEnd = false;
	if (OwnerController)
	{
		OwnerController->SetBBTargetLocation(Command.TargetLocation);
	}
}

void UPalWorkComponent_Location::WorkEvent(const FPalCommand& Command)
{
	WorkCancel();
}

void UPalWorkComponent_Location::WorkEnd(const FPalCommand& Command)
{
	WorkCancel();
}

void UPalWorkComponent_Location::WorkCancel()
{
	if (!bIsWorking)
		return;
	bIsWorkEnd = true;
	bIsWorking = false;
	if (OwnerController)
	{
		UE_LOG(LogTemp, Log, TEXT("UPalWorkComponent_Location :: WorkEvent %s"), * OwnerController->GetName());
		OwnerController->ResetMove();
	}
}
