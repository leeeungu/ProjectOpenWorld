#include "GameBase/MetaData/AMDLoop_Falling.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAnimLoopObject_Falling::Initialize(UAnimInstance* Animinstance, UAMDLoop* MetaData)
{
	Super::Initialize(Animinstance, MetaData);
	if (!OwnerAniminstance || !OwnerAniminstance->TryGetPawnOwner())
	{
		return;
	}
	Movement = Cast<UCharacterMovementComponent>(OwnerAniminstance->TryGetPawnOwner()->GetMovementComponent());
}

bool UAnimLoopObject_Falling::IsLoop() const
{
	if (Movement)
	{
		return Movement->IsFalling();
	}
	return false;
}

UAMDLoop_Falling::UAMDLoop_Falling(const FObjectInitializer& ObjectInitializer) : 
	UAMDLoop(ObjectInitializer)
{
	InstanceClass = UAnimLoopObject_Falling::StaticClass();
}
