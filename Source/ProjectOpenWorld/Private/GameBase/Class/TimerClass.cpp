#include "GameBase/Class/TimerClass.h"

void TimerClass::StartTimer(float InMaxTime) 
{
	MaxTime = InMaxTime;
	CurrentTime = 0.0f;
	bIsActive = true;
	bIsFinished = false;
}

void TimerClass::StopTimer() 
{
	bIsActive = false;
}

void TimerClass::UpdateTimer(float DeltaTime)
{
	if (!bIsActive)
		return;

	CurrentTime += DeltaTime;
	if (CurrentTime >= MaxTime)
	{
		bIsActive = false;
		bIsFinished = true;
	}
}
