#pragma once

#include "CoreMinimal.h"
#include "Pal/Component/PalCommandComponent.h"
#include "PalAllyCommandComponent.generated.h"

class UPalCommandExecutorBase;
UCLASS()
class PROJECTOPENWORLD_API UPalAllyCommandComponent : public UPalCommandComponent
{
	GENERATED_BODY()
private:
	TArray< TArray<TObjectPtr<UPalCommandExecutorBase>>> CommandExecutors{};
	UPalCommandExecutorBase* CurrentExcute{};
public:
	UPalAllyCommandComponent()  = default;
protected:
	virtual void BeginPlay() override;
public:
	virtual void OnStartCurrentCommand() override;
	virtual void OnFinishedCurrentCommand() override;
};
