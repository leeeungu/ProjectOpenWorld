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

void UInteractionComponent::ResetInteractionTarget(AActor* DestroyedActor)
{
	if (InteractionTarget == DestroyedActor)
	{
		InteractionTarget = nullptr;
		bIsInteraction = false;
	}
}

void UInteractionComponent::SetInteractionTarget(TScriptInterface<IInteractionInterface> NewTarget)
{
	if (NewTarget && NewTarget.GetObject())
	{
		AActor* Target = Cast< AActor>(NewTarget.GetObject());
		if (Target)
		{
			Target->OnDestroyed.AddDynamic(this, &UInteractionComponent::ResetInteractionTarget);
		}
		InteractionTarget = NewTarget;
	}
	else
	{
		InteractionTarget = nullptr;
		bIsInteraction = false;
	}
	
}

void UInteractionComponent::OnInteractionStart()
{
	if (InteractionTarget && InteractionTarget.GetObject() && InteractionTarget)
	{
		bIsInteraction = true;
		IInteractionInterface::Execute_OnInteractionStart(InteractionTarget.GetObject(), OwnerCharacter.Get());
	}
}

void UInteractionComponent::OnInteractionTriggered()
{
	if(InteractionTarget && InteractionTarget.GetObject() && bIsInteraction)
	{
		IInteractionInterface::Execute_OnInteraction(InteractionTarget.GetObject(), OwnerCharacter.Get());
	}
}

void UInteractionComponent::OnInteractionCompleted()
{
	if (InteractionTarget && InteractionTarget.GetObject())
	{
		bIsInteraction = false;
		IInteractionInterface::Execute_OnInteractionEnd(InteractionTarget.GetObject(), OwnerCharacter.Get());
	}
	else
	{
		bIsInteraction = false;
		InteractionTarget = nullptr;
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