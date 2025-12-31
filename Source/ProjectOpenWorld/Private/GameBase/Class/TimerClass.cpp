#include "GameBase/Class/TimerClass.h"

void TimerClass::InitTimer(float InMaxTime)
{
	MaxTime = InMaxTime;
	CurrentTime = 0.0f;
	bIsFinished = false;
}

void TimerClass::StartTimer()
{
	bIsActive = true;
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
