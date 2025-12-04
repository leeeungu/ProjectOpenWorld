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
public:
	UPalCommandExecutorBase() = default;

	virtual void Initialize(UPalCommandComponent* CommandComp);
	virtual void StartCommand(const FPalCommand& Command) {}
	virtual void TickCommand(float DeltaTime) {}
	// 강제 command 종료
	virtual void Abort() {} 

protected:
	void EndCommand();
};
