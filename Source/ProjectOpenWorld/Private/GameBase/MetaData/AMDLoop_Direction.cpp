#include "GameBase/MetaData/AMDLoop_Direction.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameBase/BaseCharacter.h"

UAMDLoop_Direction::UAMDLoop_Direction(const FObjectInitializer& ObjectInitializer) : 
	UAMDLoop(ObjectInitializer)
{
	InstanceClass = UAnimLoopObject_Direction::StaticClass();
}

void UAnimLoopObject_Direction::Initialize(UAnimInstance* Animinstance, UAMDLoop* MetaData)
{
	Super::Initialize(Animinstance, MetaData);
	OwnerPawn = OwnerAniminstance ? OwnerAniminstance->TryGetPawnOwner() : nullptr;
	if (!OwnerPawn.IsValid())
		return;
	CurrenDistance = 0.0f;
	if (UAMDLoop_Direction * Data = Cast< UAMDLoop_Direction>(LoopMetaData))
	{
		bWorldDirection = Data->GetWorldDirection();
		InitialActorDirection = Data->GetMoveDirection().GetSafeNormal();
		bControllerRotattion = Data->IsControllerRotation();
		MoveWorldDirection = InitialActorDirection;
		if (!bWorldDirection)
		{
			if(bControllerRotattion)
				MoveWorldDirection = OwnerPawn->GetControlRotation().RotateVector(MoveWorldDirection);
			else
				MoveWorldDirection = OwnerPawn->GetActorRotation().RotateVector(MoveWorldDirection);
		}
		MoveSpeed = Data->GetMoveSpeed();
		MoveDistance = Data->GetMoveDistance();
		CurrenDistance = MoveDistance;
		
		if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OwnerPawn.Get()))
		{
			if (bControllerRotattion)
			{
				BaseCharacter->UseControllerDesiredRotation();
			}
			else
			{
				BaseCharacter->UseOrientRotationToMovement();
			}
		}
	}
		MoveDistance = 0;
}
void UAnimLoopObject_Direction::UpdateLoop(float DeltaTime)
{
	if (!OwnerPawn.IsValid())
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
		MoveWorldDirection = InitialActorDirection.GetSafeNormal();
		if (!bWorldDirection)
		{
			if (bControllerRotattion)
				MoveWorldDirection = OwnerPawn->GetControlRotation().RotateVector(MoveWorldDirection);
			else
				MoveWorldDirection = OwnerPawn->GetActorRotation().RotateVector(MoveWorldDirection);
		}
		OwnerPawn->AddActorWorldOffset(MoveWorldDirection * DeltaMove, true, nullptr, ETeleportType::None);
	}
}

bool UAnimLoopObject_Direction::IsLoop() const
{
	return CurrenDistance > 0;
}

