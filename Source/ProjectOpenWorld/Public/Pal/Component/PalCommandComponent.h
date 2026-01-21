#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Data/PalCommandData.h"
#include "Pal/CommandExecutor/PalCommandExecutorBase.h"
#include "PalCommandComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCommandEvent,AActor*, PalActor,  FPalCommand, Command);

struct FAIRequestID;
namespace EPathFollowingResult { enum Type : int; }

UCLASS()
class PROJECTOPENWORLD_API UPalCommandComponent_Pre : public UActorComponent
{
	GENERATED_BODY()
private:
	enum CommandSize
	{
		MaxSize = 3
	};
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "True"))
	TArray<FPalCommand> CommandPool{};
	TQueue<FPalCommand*> QueueEmpty{};
	TQueue<FPalCommand*> QueueCommand{};
	FPalCommand DummyCommand{};
	FPalCommand* LastCommand{};
	bool (UPalCommandComponent_Pre::* PushCommandFunc)(const FPalCommand&);
	bool bCommandStarted{};
protected:
	UPROPERTY(VisibleAnywhere, Category = "PalCommand")
	FPalCommand CurrentCommandData{};
	FPalCommand* CurrentCommand{};
protected:
	// garbage 문제로 stringobj를 사용
	TArray< TArray<TStrongObjectPtr<UPalCommandExecutorBase>>> CommandExecutors{};
	UPROPERTY()
	TObjectPtr<UPalCommandExecutorBase> CurrentExcute{};
public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "PalCommand")
	FOnCommandEvent OnCommandFinished{};
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "PalCommand")
	FOnCommandEvent OnCommandStarted{};
public:
	UPalCommandComponent_Pre();
protected:
	void ResetCommand(FPalCommand& pData);
	void ResetCurrentCommand();
	virtual void BeginPlay() override;
	 bool PopCommand();

	virtual bool PushCommand_Default(const FPalCommand& NewCommand);
	 bool PushCommand_DequqOld(const FPalCommand& NewCommand);
	 void SetPushCommandFunc(bool (UPalCommandComponent_Pre::* Func)(const FPalCommand&));

	UFUNCTION()
	 void FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result);
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool IsValidCommand() { return CurrentCommand != &DummyCommand; }
	virtual const FPalCommand* GetCurrentCommand_C() const { return CurrentCommand; }
	virtual FPalCommand GetCurrentCommand() const { return *CurrentCommand; }

	UFUNCTION(BlueprintPure, Category = "PalCommand")
	FORCEINLINE EPalCommandKind GetCurrentCommandKind() const { return CurrentCommand->CommandKind; }

	UFUNCTION(BlueprintPure, Category = "PalCommand")
	FORCEINLINE uint8 GetCurrentSubCommandType() const { return CurrentCommand->SubCommandType; }

	UFUNCTION(BlueprintCallable, Category = "PalCommand")
	bool PushCommand(const FPalCommand& NewCommand);

	UFUNCTION(BlueprintCallable, Category = "PalCommand")
	virtual void FinishCommand(); // == stop

	UFUNCTION(BlueprintCallable, Category = "PalCommand")
	virtual void ResetCommandQue();

	virtual void OnStartCurrentCommand() {}
	virtual void OnFinishedCurrentCommand() {}
};


UCLASS()
class PROJECTOPENWORLD_API UPalCommandComponent : public UPalCommandComponent_Pre
{
	GENERATED_BODY()
protected:
	virtual bool PushCommand_Default(const FPalCommand& NewCommand) override;

public:
	virtual bool IsValidCommand() override;

	template <typename T>
	void CreateNewExcutor(EPalCommandKind CommandType, uint8 SubType, FName Name = NAME_None);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void FinishCommand() override; // == stop
	virtual void ResetCommandQue() override;
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
