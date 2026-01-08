#pragma once

#include "CoreMinimal.h"
#include "Pal/CommandExecutor/PalCommandExecutorBase.h"
#include "PalCommandExecutor_DetectPlayer.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UPalCommandExecutor_DetectPlayer : public UPalCommandExecutorBase
{
	GENERATED_BODY()
public:
	UPalCommandExecutor_DetectPlayer() = default;

	virtual void Initialize(UPalCommandComponent* CommandComp)  override;
	virtual bool StartCommand(const FPalCommand& Command) override;
	virtual void WorkCommand()  override;
	virtual bool CheckCommandValid()   override;
	virtual void TickCommand(float DeltaTime)  override;
	// 강제 command  종료;
	virtual void Abort()  override;
};
