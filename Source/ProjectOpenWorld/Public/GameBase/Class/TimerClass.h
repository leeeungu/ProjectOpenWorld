#pragma once

#include "CoreMinimal.h"

class TimerClass
{
	float CurrentTime = 0.0f;
	float MaxTime = 0.0f;
	bool bIsActive = false;
	bool bIsFinished = false;
public:
	TimerClass() = default;
	virtual ~TimerClass() = default;

	void StartTimer(float InMaxTime);
	void StopTimer();
	void UpdateTimer(float DeltaTime);
	float GetCurrentTime() const { return CurrentTime; }
	bool IsFinished() const { return bIsFinished; }

};
