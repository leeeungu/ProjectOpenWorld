#include "Pal/Component/Work/PalWorkComponent_Mining.h"


UPalWorkComponent_Mining::UPalWorkComponent_Mining() : Super()
{
	PalCommand = EPalCommandKind::Work;
	PalSubCommand = static_cast<uint8>(ESubWorkType::Mining);
}

void UPalWorkComponent_Mining::WorkStart(const FPalCommand& Command)
{
	if (bIsWorking)
		return;
	bIsWorking = true;
	//if (!InteractionComp->IsInteracting())
	//{
	//	OwnerPal->SetActionStarted(true);
	//	InteractionComp->SetInteractionTarget(Command->pTarget.Get());
	//	InteractionComp->OnInteractionStart();
	//}
}

void UPalWorkComponent_Mining::WorkEnd(const FPalCommand& Command)
{
}

void UPalWorkComponent_Mining::WorkCancel()
{
}
