#include "Pal/Component/Work/PalWorkComponent.h"
#include "Resource/Interface/ResourceInterface.h"


UPalWorkComponent::UPalWorkComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPalWorkComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerPal = Cast<ABaseCreature>(GetOwner());

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		OwnerController = Cast<APalAIController>(OwnerCharacter->GetController());
	}
}

//void UPalWorkComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//}

