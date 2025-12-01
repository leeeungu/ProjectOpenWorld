#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PalCommandComponent.generated.h"

UENUM(BlueprintType)
enum class EPalCommandKind : uint8
{
	None,
	Move,
	Attack,
	Interact,
	Work,
};


USTRUCT(BlueprintType)
struct FPalCommand
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPalCommandKind CommandKind{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* pInstigatorActor{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* pTarget{};
};

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
protected:
	FPalCommand* CurrentCommand{};
	void (UPalCommandComponent::* PushCommandFunc)(const FPalCommand&);
public:	
	UPalCommandComponent();
private:
	void ResetCommand(FPalCommand& pData);
	void ResetCurrentCommand();
protected:
	virtual void BeginPlay() override;

	void PushCommand(const FPalCommand& NewCommand);
	void PopCommand();

	// use if you want deque Old Command when pushed new Command
	void PushCommand_DequqOld(const FPalCommand& NewCommand);
public:	
	UFUNCTION(BlueprintPure)
	FPalCommand GetCurrentCommand() const { return *CurrentCommand;  }

	UFUNCTION(BlueprintCallable)
	void PushCommand_Attack(AActor* pInstigatorActor, AActor* pTarget);

	UFUNCTION(BlueprintCallable)
	void FinishCommand();
	
	virtual void OnStartCurrentCommand()  {}
	virtual void OnEndCurrentCommand()  {}
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
