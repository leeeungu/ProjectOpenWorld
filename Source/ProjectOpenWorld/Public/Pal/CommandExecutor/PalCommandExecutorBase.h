#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Pal/Data/PalCommandData.h"
#include "PalCommandExecutorBase.generated.h"

class UPalCommandComponent;
UCLASS()
class PROJECTOPENWORLD_API UPalCommandExecutorBase : public UObject
{
	GENERATED_BODY()
protected:
	UPalCommandComponent* OwnerCommandComp{};
	bool IsCommandStarted{ false };

public:
	UPalCommandExecutorBase() = default;

	virtual void Initialize(UPalCommandComponent* CommandComp);
	virtual bool StartCommand(const FPalCommand& Command) { return false; }
	virtual void WorkCommand() {}
	virtual bool CheckCommandValid() { return false; }
	virtual void TickCommand(float DeltaTime) {}
	// 강제 command 종료
	virtual void Abort() {} 

	void ResetStarted() { IsCommandStarted = false; }
protected:
	void EndCommand();
};
