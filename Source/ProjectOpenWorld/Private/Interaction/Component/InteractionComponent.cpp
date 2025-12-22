#include "Interaction/Component/InteractionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/InteractionInterface.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

//void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast< ACharacter>(GetOwner());
}

void UInteractionComponent::SetInteractionTarget(TScriptInterface<IInteractionInterface> NewTarget)
{
	if (!NewTarget || !NewTarget.GetObject())
		return;
	InteractionTarget = NewTarget;
}

void UInteractionComponent::OnInteractionStart()
{
	if (InteractionTarget)
	{
		bIsInteraction = true;
		IInteractionInterface::Execute_OnInteractionStart(InteractionTarget.GetObject(), OwnerCharacter.Get());
	}
}

void UInteractionComponent::OnInteractionTriggered()
{
	if (InteractionTarget && bIsInteraction)
	{
		IInteractionInterface::Execute_OnInteraction(InteractionTarget.GetObject(), OwnerCharacter.Get());
	}
}

void UInteractionComponent::OnInteractionCompleted()
{
	if (InteractionTarget)
	{
		bIsInteraction = false;
		IInteractionInterface::Execute_OnInteractionEnd(InteractionTarget.GetObject(), OwnerCharacter.Get());
	}
}

void UInteractionComponent::OnActorCancel()
{
	if (InteractionTarget && InteractionTarget.GetObject())
	{
		IInteractionInterface::Execute_OnInteractionCanceled(InteractionTarget.GetObject(), OwnerCharacter.Get());
		if (bIsInteraction)
			IInteractionInterface::Execute_OnInteractionEnd(InteractionTarget.GetObject(), OwnerCharacter.Get());
		else
			IInteractionInterface::Execute_OnEndDetected(InteractionTarget.GetObject(), OwnerCharacter.Get());
		bIsInteraction = false;
		InteractionTarget = nullptr;
	}
}

AActor* UInteractionComponent::GetTargetActor() const
{ 
	if (!InteractionTarget || !InteractionTarget.GetObject()) 
		return nullptr; 
	return Cast<AActor>(InteractionTarget.GetObject()); 
}