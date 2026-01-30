#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Data/PalCommandData.h"
#include "Pal/CommandExecutor/PalCommandExecutorBase.h"
#include "PalCommandComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCommandEvent,AActor*, PalActor,  FPalCommand, Command);

struct FAIRequestID;
namespace EPathFollowingResult { enum Type : int; }

//UCLASS()
//class PROJECTOPENWORLD_API UPalCommandComponent_Pre : public UActorComponent
//{
//	GENERATED_BODY()
//private:
//	enum CommandSize
//	{
//		MaxSize = 3
//	};
//	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "True"))
//	TArray<FPalCommand> CommandPool{};
//	TQueue<FPalCommand*> QueueEmpty{};
//	TQueue<FPalCommand*> QueueCommand{};
//	FPalCommand DummyCommand{};
//	FPalCommand* LastCommand{};
//	bool (UPalCommandComponent_Pre::* PushCommandFunc)(const FPalCommand&);
//	bool bCommandStarted{};
//
//protected:
//	// garbage 문제로 stringobj를 사용
//	TArray< TArray<TStrongObjectPtr<UPalCommandExecutorBase>>> CommandExecutors{};
//	UPROPERTY()
//	TObjectPtr<UPalCommandExecutorBase> CurrentExcute{};
//
//public:
//	UPalCommandComponent_Pre();
//protected:
//	void ResetCommand(FPalCommand& pData);
//	void ResetCurrentCommand();
//	 bool PopCommand();
//
////	virtual bool PushCommand_Default(const FPalCommand& NewCommand);
////	 bool PushCommand_DequqOld(const FPalCommand& NewCommand);
////	 void SetPushCommandFunc(bool (UPalCommandComponent_Pre::* Func)(const FPalCommand&));
////

////public:
////	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
////
//
////
////	UFUNCTION(BlueprintCallable, Category = "PalCommand")
////	virtual void ResetCommandQue();
//
//	virtual void OnStartCurrentCommand() {}
//	virtual void OnFinishedCurrentCommand() {}
//};

class UPalWorkComponent;

UCLASS()
class PROJECTOPENWORLD_API UPalCommandComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, Category = "PalCommand")
	FPalCommand CurrentCommandData{};
	FPalCommand* CurrentCommand{};

	TObjectPtr<UPalWorkComponent> CurrentExcute{};

	TMap<EPalCommandKind, TArray<TObjectPtr<UPalWorkComponent>>> CommandExecutors{};
protected:
	virtual bool PushCommand_Default(const FPalCommand& NewCommand) ;
	virtual void BeginPlay() override;
public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "PalCommand")
	FOnCommandEvent OnCommandFinished{};
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "PalCommand")
	FOnCommandEvent OnCommandStarted{};

public:
	UPalCommandComponent();
	virtual bool IsValidCommand();

	//template <typename T>
	//void CreateNewExcutor(EPalCommandKind CommandType, uint8 SubType, FName Name = NAME_None);


	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable, Category = "PalCommand")
	bool PushCommand(const FPalCommand& NewCommand);
	UFUNCTION(BlueprintCallable, Category = "PalCommand")
	virtual void FinishCommand(); // == stop
	UFUNCTION(BlueprintCallable, Category = "PalCommand")
	virtual void ResetCommandQue();
	// Cant nullptr
	virtual const FPalCommand* GetCurrentCommand_C() const { return &CurrentCommandData; }
	virtual FPalCommand GetCurrentCommand() const { return CurrentCommandData; }

	UFUNCTION(BlueprintPure, Category = "PalCommand")
	FORCEINLINE EPalCommandKind GetCurrentCommandKind() const { return CurrentCommandData.CommandKind; }

	UFUNCTION(BlueprintPure, Category = "PalCommand")
	FORCEINLINE uint8 GetCurrentSubCommandType() const { return CurrentCommandData.SubCommandType; }


	virtual void OnStartCurrentCommand() {}
	virtual void OnFinishedCurrentCommand() {}

protected:
	UFUNCTION()
	void FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};


//template<typename T>
//inline void UPalCommandComponent::CreateNewExcutor(EPalCommandKind CommandType, uint8 SubType, FName Name)
//{
//	if (!CommandExecutors.IsValidIndex((uint8)CommandType) || !CommandExecutors[(uint8)CommandType].IsValidIndex(SubType))
//		return;
//	TStrongObjectPtr<T> Base = TStrongObjectPtr<T>(NewObject<T>(this, Name));
//	CommandExecutors[(uint8)CommandType][SubType] = Base;
//	Base->Initialize(this);
//
//}
