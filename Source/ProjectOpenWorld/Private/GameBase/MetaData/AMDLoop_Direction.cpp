#include "GameBase/MetaData/AMDLoop_Direction.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/Character/PalBaseCharacter.h"

UAMDLoop_Direction::UAMDLoop_Direction(const FObjectInitializer& ObjectInitializer) : 
	UAMDLoop(ObjectInitializer)
{
	InstanceClass = UAnimLoopObject_Direction::StaticClass();
}

void UAnimLoopObject_Direction::Initialize(UAnimInstance* Animinstance, UAMDLoop* MetaData)
{
	Super::Initialize(Animinstance, MetaData);
	OwnerPawn = OwnerAniminstance ? OwnerAniminstance->TryGetPawnOwner() : nullptr;
	CurrenDistance = 0.0f;
	if (UAMDLoop_Direction * Data = Cast< UAMDLoop_Direction>(LoopMetaData))
	{
		bWorldDirection = Data->GetWorldDirection();
		InitialActorDirection = Data->GetMoveDirection().GetSafeNormal();
		bControllerRotattion = Data->IsControllerRotation();
		MoveWorldDirection = InitialActorDirection;
	
		MoveSpeed = Data->GetMoveSpeed();
		CurrenDistance = Data->GetMoveDistance();
		if (OwnerPawn.IsValid())
		{
			if (!bWorldDirection)
			{
				if (bControllerRotattion)
					MoveWorldDirection = OwnerPawn->GetControlRotation().RotateVector(MoveWorldDirection);
				else
					MoveWorldDirection = OwnerPawn->GetActorRotation().RotateVector(MoveWorldDirection);
			}
			if (APalBaseCharacter* BaseCharacter = Cast<APalBaseCharacter>(OwnerPawn.Get()))
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
	}
}

void UAnimLoopObject_Direction::UpdateLoop(float DeltaTime)
{
	bLoop = CurrenDistance > 0;
	if (bLoop)
	{
		float DeltaMove = MoveSpeed * DeltaTime;
		if (DeltaMove > CurrenDistance)
			DeltaMove = CurrenDistance;
		CurrenDistance -= DeltaMove;
		MoveWorldDirection = InitialActorDirection.GetSafeNormal();
		if (OwnerPawn.IsValid())
		{
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
}

bool UAnimLoopObject_Direction::IsLoop() const
{
	return CurrenDistance > 0;
}

