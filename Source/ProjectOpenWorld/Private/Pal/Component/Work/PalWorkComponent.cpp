#include "Pal/Component/Work/PalWorkComponent.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Controller/PalAIController.h"
#include "Resource/Interface/ResourceInterface.h"


UPalWorkComponent::UPalWorkComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPalWorkComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerPal = Cast<ABaseCreature>(GetOwner());;
	if(OwnerPal)
		OwnerController = Cast<APalAIController>(OwnerPal->GetController());
}

//void UPalWorkComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//}

