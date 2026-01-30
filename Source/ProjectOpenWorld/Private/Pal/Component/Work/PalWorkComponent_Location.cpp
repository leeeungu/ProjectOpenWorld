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
	UE_LOG(LogTemp, Log, TEXT("Location Work Start"));
	bIsWorking = true;
	if (OwnerController)
	{
		OwnerController->SetBBTargetLocation(Command.TargetLocation);
	}
}

void UPalWorkComponent_Location::WorkEvent(const FPalCommand& Command)
{
	UE_LOG(LogTemp, Log, TEXT("Location Work Event"));
	if (FVector::Distance(Command.TargetLocation, OwnerPal->GetActorLocation()) >= 100.0f)
	{
		if (OwnerController)
		{
			OwnerController->SetBBTargetLocation(Command.TargetLocation);
		}
	}
}

void UPalWorkComponent_Location::WorkEnd(const FPalCommand& Command)
{
	WorkCancel();
}

void UPalWorkComponent_Location::WorkCancel()
{
	if (!bIsWorking)
		return;
	bIsWorking = false;
	if (OwnerController)
	{
		OwnerController->ResetMove();
	}
}
