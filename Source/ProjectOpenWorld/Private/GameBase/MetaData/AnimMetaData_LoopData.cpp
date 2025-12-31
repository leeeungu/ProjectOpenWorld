#include "GameBase/MetaData/AnimMetaData_LoopData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Curves/CurveVector.h"
//
//void UAnimMetaData_LoopData::InitUpdateMetaData_Implementation(ACharacter* pOwner, float Start, float Loop, float End)
//{
//	if (LoopCount < 0)
//	{
//		LoopCount = 0;
//	}
//	TempCount = 0;
//	//AnimationPlayLength = UpdateLength;
//	//CurrentTime = 0;
//	OwnerCharacter = pOwner;
//	if (OwnerCharacter)
//	{
//		//OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
//		//OwnerCharacter->LaunchCharacter(OwnerCharacter->GetActorForwardVector() * (MoveSpeed), false, false);
//	}
//
//	StartLength = Start;
//	LoopLength = Loop;
//	TotalLoopLength = LoopLength * LoopCount;
//	EndLength = End;
//	TotalLength = StartLength + TotalLoopLength + EndLength;
//
//	Timer.StartTimer(TotalLength);
//	IsMoving = true;
//}
//
//void UAnimMetaData_LoopData::UpdateMetaData_Implementation(float DeltaTime)
//{
//	Timer.UpdateTimer(DeltaTime);
//	
//	if (Timer.IsFinished())
//	{
//		IsMoving = false;
//		//TempCount--;
//		//Timer.StartTimer(AnimationPlayLength - (Timer.GetCurrentTime() - AnimationPlayLength));
//		return;
//	}
//	float curTime = Timer.GetCurrentTime();
//	if (StartLength < curTime  && curTime < StartLength + TotalLoopLength && LoopCount >= 0)
//	{
//		if (curTime >= StartLength + LoopLength * (TempCount + 1))
//		{
//			TempCount++;
//		}
//		if (TempCount <= LoopCount)
//		{
//			curTime -= LoopLength * TempCount;
//		}
//	}
//	else if (StartLength + TotalLoopLength <= curTime)
//	{
//		TempCount = LoopCount;
//		curTime -= TotalLoopLength;
//	}
//	if (OwnerCharacter)
//	{
//		FVector Offset = OwnerCharacter->GetActorRotation().Quaternion() * MoveData->GetVectorValue(curTime);
//		OwnerCharacter->AddActorWorldOffset(Offset);
//	}
//	if (StartLength < Timer.GetCurrentTime() && Timer.GetCurrentTime() < StartLength + TotalLoopLength)
//	{
//		UE_LOG(LogTemp, Log, TEXT("SubMoveTime %d  %s"), TempCount, *MoveData->GetVectorValue(curTime).ToString());
//	}
//	
//}

void UAnimMetaData_LoopData::InitLoopMetaData_Implementation(ACharacter* pOwner, float Start, float Loop)
{
	TempCount = 0;
	StartLength = Start;
	LoopLength = Loop;
	TotalLength = StartLength + LoopLength * LoopCount;
	Timer.InitTimer(TotalLength);
	Timer.StartTimer();
}

void UAnimMetaData_LoopData::LoopUpdate_Implementation(float DeltaTime)
{
	if (Timer.IsFinished())
	{
		TempCount = LoopCount;
		return;
	}
	Timer.UpdateTimer(DeltaTime);
	float curTime = Timer.GetCurrentTime();
	if (curTime >= StartLength + LoopLength * TempCount)
	{
		TempCount++;
	}
}
