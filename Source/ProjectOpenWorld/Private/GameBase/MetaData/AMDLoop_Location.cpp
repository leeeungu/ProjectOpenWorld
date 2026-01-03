#include "GameBase/MetaData/AMDLoop_Location.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Curves/CurveVector.h"

UAMDLoop_Location::UAMDLoop_Location(const FObjectInitializer& ObjectInitializer) : 
	UAMDLoop(ObjectInitializer)
{
	InstanceClass = UAnimLoopObject_Location::StaticClass();
}

bool UAnimLoopObject_Location::CheckDisSq()
{
	return FVector::DistSquared(OwnerAniminstance->TryGetPawnOwner()->GetActorLocation(), MoveLocation) > 100.f*100.0f;
}


void UAnimLoopObject_Location::Initialize(UAnimInstance* Animinstance, UAMDLoop* MetaData)
{
	Super::Initialize(Animinstance, MetaData);
	if (!OwnerAniminstance || !OwnerAniminstance->TryGetPawnOwner())
		return;
	InitDisSq = 0.0f;
	MoveLocation = FVector::ZeroVector;
	MoveDir = FVector::ZeroVector;
	StartLocation = FVector::ZeroVector;

	if (UAMDLoop_Location* Data = Cast< UAMDLoop_Location>(LoopMetaData))
	{
		MoveLocation = Data->GetMoveLocation();
		StartLocation = OwnerAniminstance->TryGetPawnOwner()->GetActorLocation();
		InitDisSq =  FVector::DistSquared(StartLocation, MoveLocation);
		MoveDir = (MoveLocation - StartLocation).GetSafeNormal();
		bLoop = CheckDisSq();
	}
	else if (OwnerAniminstance && OwnerAniminstance->TryGetPawnOwner())
	{
		MoveLocation = OwnerAniminstance->TryGetPawnOwner()->GetActorLocation();
	}
}

void UAnimLoopObject_Location::UpdateLoop(float DeltaTime)
{
	if (!OwnerAniminstance || !OwnerAniminstance->TryGetPawnOwner())
	{
		bLoop = false;
		return;
	}
	bLoop = CheckDisSq();
	if (bLoop)
	{
		float Radio = FVector::DistSquared(OwnerAniminstance->TryGetPawnOwner()->GetActorLocation(), StartLocation) / InitDisSq;
		if (UAMDLoop_Location* Data = Cast< UAMDLoop_Location>(LoopMetaData))
		{
			float Speed = Data->GetMoveSpeed();
			if (UCurveVector* SpeedData = Data->GetSpeedData())
			{
				Speed = SpeedData->GetVectorValue(Radio).X;
			}
			OwnerAniminstance->TryGetPawnOwner()->AddActorWorldOffset(MoveDir * Speed * DeltaTime);
		}
		else
		{
			bLoop = false;

		}
	}
}
