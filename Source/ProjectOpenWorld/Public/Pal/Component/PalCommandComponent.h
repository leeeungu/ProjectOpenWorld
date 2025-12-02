#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Data/PalCommandData.h"
#include "PalCommandComponent.generated.h"


UCLASS()
class PROJECTOPENWORLD_API UPalCommandComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	enum CommandSize
	{
		MaxSize = 10
	};
	TArray<FPalCommand> CommandPool{};
	TQueue<FPalCommand*> QueueEmpty{};
	TQueue<FPalCommand*> QueueCommand{};
	FPalCommand DummyCommand{};
	FPalCommand* CurrentCommand{};
	FPalCommand* LastCommand{};
	void (UPalCommandComponent::* PushCommandFunc)(const FPalCommand&);
public:	
	UPalCommandComponent();
private:
	void ResetCommand(FPalCommand& pData);
	void ResetCurrentCommand();
protected:
	virtual void BeginPlay() override;

	void PopCommand();

	void PushCommand_Default(const FPalCommand& NewCommand);
	void PushCommand_DequqOld(const FPalCommand& NewCommand);
	void SetPushCommandFunc(void (UPalCommandComponent::* Func)(const FPalCommand&));
	inline FPalCommand* GetCurrentCommand_C() const { return CurrentCommand;  }
	bool IsValidCommand() { return CurrentCommand != &DummyCommand; }
public:	
	FPalCommand GetCurrentCommand() const { return *CurrentCommand;  }

	UFUNCTION(BlueprintCallable)
	void PushCommand(const FPalCommand& NewCommand);

	UFUNCTION(BlueprintCallable)
	void FinishCommand();
	
	virtual void OnStartCurrentCommand()  {}
	// called by FinishCommand
	virtual void OnFinishedCurrentCommand()  {}
};
