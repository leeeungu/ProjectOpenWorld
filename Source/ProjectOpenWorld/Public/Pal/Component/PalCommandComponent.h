#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Data/PalCommandData.h"
#include "Pal/CommandExecutor/PalCommandExecutorBase.h"
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
protected:
	TArray< TArray<TStrongObjectPtr<UPalCommandExecutorBase>>> CommandExecutors{};
	UPalCommandExecutorBase* CurrentExcute{};
public:	
	UPalCommandComponent();
private:
	void ResetCommand(FPalCommand& pData);
	void ResetCurrentCommand();
protected:
	virtual void BeginPlay() override;

	template <typename T>
	void CreateNewExcutor(EPalCommandKind CommandType, uint8 SubType, FName Name = NAME_None);
	void PopCommand();

	void PushCommand_Default(const FPalCommand& NewCommand);
	void PushCommand_DequqOld(const FPalCommand& NewCommand);
	void SetPushCommandFunc(void (UPalCommandComponent::* Func)(const FPalCommand&));
public:	
	bool IsValidCommand() { return CurrentCommand != &DummyCommand; }
	inline const FPalCommand* GetCurrentCommand_C() const { return CurrentCommand;  }
	FPalCommand GetCurrentCommand() const { return *CurrentCommand;  }

	UFUNCTION(BlueprintPure, Category = "PalCommand")
	FORCEINLINE EPalCommandKind GetCurrentCommandKind() const { return CurrentCommand->CommandKind; }

	UFUNCTION(BlueprintPure, Category = "PalCommand")
	FORCEINLINE uint8 GetCurrentSubCommandType() const { return CurrentCommand->SubCommandType; }

	UFUNCTION(BlueprintCallable, Category = "PalCommand")
	void PushCommand(const FPalCommand& NewCommand);

	UFUNCTION(BlueprintCallable, Category = "PalCommand")
	void FinishCommand();
	
	virtual void OnStartCurrentCommand()  {}
	// called by FinishCommand
	virtual void OnFinishedCurrentCommand()  {}
};
template<typename T>
inline void UPalCommandComponent::CreateNewExcutor(EPalCommandKind CommandType, uint8 SubType, FName Name)
{
	if (!CommandExecutors.IsValidIndex((uint8)CommandType) || !CommandExecutors[(uint8)CommandType].IsValidIndex(SubType))
		return;
	TStrongObjectPtr<T> Base = TStrongObjectPtr<T>(NewObject<T>(this, Name));
	CommandExecutors[(uint8)CommandType][SubType] = Base;
	Base->Initialize(this);

}
