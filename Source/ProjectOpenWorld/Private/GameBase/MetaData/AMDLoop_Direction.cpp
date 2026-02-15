#include "GameBase/MetaData/AMDLoop_Direction.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"

UAMDLoop_Direction::UAMDLoop_Direction(const FObjectInitializer& ObjectInitializer) : 
	UAMDLoop(ObjectInitializer)
{
	InstanceClass = UAnimLoopObject_Direction::StaticClass();
}

void UAnimLoopObject_Direction::Initialize(UAnimInstance* Animinstance, UAMDLoop* MetaData)
{
	Super::Initialize(Animinstance, MetaData);
	if (!OwnerAniminstance || !OwnerAniminstance->TryGetPawnOwner())
		return;
	CurrenDistance = 0.0f;
	if (UAMDLoop_Direction * Data = Cast< UAMDLoop_Direction>(LoopMetaData))
	{
		bWorldDirection = Data->GetWorldDirection();
		MoveWorldDirection = Data->GetMoveDirection().GetSafeNormal();
		InitialActorDirection = MoveWorldDirection;
		if (!bWorldDirection)
		{
			MoveWorldDirection = OwnerAniminstance->TryGetPawnOwner()->GetControlRotation().RotateVector(MoveWorldDirection);
		}
		MoveSpeed = Data->GetMoveSpeed();
		MoveDistance = Data->GetMoveDistance();
		CurrenDistance = MoveDistance;
	}
	else if (OwnerAniminstance && OwnerAniminstance->TryGetPawnOwner())
	{
		MoveDistance = 0;
	}
}
void UAnimLoopObject_Direction::UpdateLoop(float DeltaTime)
{
	if (!OwnerAniminstance || !OwnerAniminstance->TryGetPawnOwner())
	{
		bLoop = false;
		return;
	}
	bLoop = CurrenDistance > 0;
	if (bLoop)
	{
		float DeltaMove = MoveSpeed * DeltaTime;
		if (DeltaMove > CurrenDistance)
			DeltaMove = CurrenDistance;
		CurrenDistance -= DeltaMove;
		MoveWorldDirection = MoveWorldDirection.GetSafeNormal();
		if (!bWorldDirection)
		{
			MoveWorldDirection = OwnerAniminstance->TryGetPawnOwner()->GetControlRotation().RotateVector(InitialActorDirection);
		}
		OwnerAniminstance->TryGetPawnOwner()->AddActorWorldOffset(MoveWorldDirection * DeltaMove, true, nullptr, ETeleportType::None);
	}
}

