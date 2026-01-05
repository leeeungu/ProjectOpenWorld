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
		MoveWorldDirection = Data->GetMoveDirection().GetSafeNormal();
		if (!Data->GetWorldDirection())
		{
			MoveWorldDirection = OwnerAniminstance->TryGetPawnOwner()->GetActorRotation().RotateVector(MoveWorldDirection);
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
		//UE_LOG(LogTemp, Warning, TEXT("DeltaMove: %f, CurrenDistance: %f"), DeltaMove, CurrenDistance);
		//FHitResult Hit{};
		OwnerAniminstance->TryGetPawnOwner()->AddActorWorldOffset(MoveWorldDirection * DeltaMove, true, nullptr, ETeleportType::None);
	/*	if (Hit.bBlockingHit)
		{
			ACharacter* Other = Cast< ACharacter>(Hit.GetActor());
			if (Other)
			{
				Other->LaunchCharacter(MoveWorldDirection * (MoveSpeed * 0.5f), true, true);
			}
		}*/
	}
}

