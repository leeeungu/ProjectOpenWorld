#include "Pal/Component/Work/PalWorkComponent_Mining.h"

#include "Interaction/Component/InteractionComponent.h"	

UPalWorkComponent_Mining::UPalWorkComponent_Mining() : Super()
{
	PalCommand = EPalCommandKind::Work;
	PalSubCommand = static_cast<uint8>(ESubWorkType::Mining);
}

void UPalWorkComponent_Mining::BeginPlay()
{
	Super::BeginPlay();
	if (OwnerPal)
		InteractionComp = OwnerPal->GetInteractionComponent();
}

bool UPalWorkComponent_Mining::CanWork()
{
	return Super::CanWork() && InteractionComp;
}

void UPalWorkComponent_Mining::WorkStart(const FPalCommand& Command)
{
	bIsArrive = true;
	bIsWorkEnd = false;
	if (OwnerController)
		OwnerController->SetBBTargetActor(Command.pTarget.Get());
}

void UPalWorkComponent_Mining::WorkEvent(const FPalCommand& Command)
{
	if (bIsWorking)
		return;
	bIsArrive = false;
	bIsWorking = true;
	if (InteractionComp && !InteractionComp->IsInteracting())
	{
		OwnerPal->SetActionStarted(true);
		InteractionComp->SetInteractionTarget(Command.pTarget.Get());
		InteractionComp->OnInteractionStart();
	}
}

void UPalWorkComponent_Mining::WorkEnd(const FPalCommand& Command)
{
	WorkCancel();
}

void UPalWorkComponent_Mining::WorkCancel()
{
	if (!bIsWorking)
		return;
	bIsWorking = false;
	bIsWorkEnd = true;
	if (InteractionComp)
	{
		InteractionComp->OnInteractionStart();
		InteractionComp->SetInteractionTarget(nullptr);
	}
}
