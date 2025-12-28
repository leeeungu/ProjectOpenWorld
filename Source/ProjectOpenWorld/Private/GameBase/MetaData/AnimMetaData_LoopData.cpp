#include "GameBase/MetaData/AnimMetaData_LoopData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAnimMetaData_LoopData::InitUpdateMetaData_Implementation(ACharacter* pOwner, float UpdateLength)
{
	TempCount = LoopCount;
	AnimationPlayLength = UpdateLength;
	//CurrentTime = 0;
	Timer.StartTimer(UpdateLength);
	OwnerCharacter = pOwner;
	if (OwnerCharacter)
	{
		OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
		OwnerCharacter->LaunchCharacter(OwnerCharacter->GetActorForwardVector() * (MoveSpeed), false, false);
	}
}

void UAnimMetaData_LoopData::UpdateMetaData_Implementation(float DeltaTime)
{
	Timer.UpdateTimer(DeltaTime);
	
	if (Timer.IsFinished())
	{
		TempCount--;
		Timer.StartTimer(AnimationPlayLength - (Timer.GetCurrentTime() - AnimationPlayLength));
	}
	if (TempCount > 0)
	{
		//if (OwnerCharacter)
		//OwnerCharacter->stopmo(OwnerCharacter->GetActorForwardVector() * (MoveSpeed * DeltaTime));
	}
}
