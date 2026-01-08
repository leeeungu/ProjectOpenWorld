#include "GameBase/MetaData/AMDLoop_Time.h"

UAMDLoop_Time::UAMDLoop_Time(const FObjectInitializer& ObjectInitializer)
{
	InstanceClass = UAnimLoopObject_Time::StaticClass();
}

void UAnimLoopObject_Time::Initialize(UAnimInstance* Animinstance, UAMDLoop* MetaData)
{
	Super::Initialize(Animinstance, MetaData);
	if (UAMDLoop_Time* Data = Cast< UAMDLoop_Time>(LoopMetaData))
	{
		LoopTime = Data->GetLoopTime();
		CurrentTime = 0.0f;
		bLoop = true;
	}
	else
	{
		bLoop = false;
	}
}

void UAnimLoopObject_Time::UpdateLoop(float DeltaTime)
{
	if (!bLoop)
		return;
	CurrentTime += DeltaTime;
	if (CurrentTime >= LoopTime)
	{
		bLoop = false;
	}
}

void UAnimLoopObject_Time::EndLoop()
{
	bLoop = false;
}
