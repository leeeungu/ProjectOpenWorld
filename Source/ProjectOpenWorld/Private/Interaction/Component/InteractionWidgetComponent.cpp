#include "Interaction/Component/InteractionWidgetComponent.h"

void UInteractionWidgetComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionWidgetComponent::OnBeginDetected_Implementation(ACharacter* pOther)
{
	SetVisibility(true);
}

void UInteractionWidgetComponent::OnEndDetected_Implementation(ACharacter* pOther)
{
	SetVisibility(false);
}

void UInteractionWidgetComponent::OnInteractionStart_Implementation(ACharacter* pOther)
{
}

void UInteractionWidgetComponent::OnInteraction_Implementation(ACharacter* pOther)
{
}

void UInteractionWidgetComponent::OnInteractionEnd_Implementation(ACharacter* pOther)
{
}

void UInteractionWidgetComponent::OnInteractionCanceled_Implementation(ACharacter* pOther)
{
}
