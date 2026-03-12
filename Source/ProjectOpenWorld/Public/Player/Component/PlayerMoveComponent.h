#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerMoveComponent.generated.h"

struct FInputActionValue;
class APlayerController;
class ACharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPlayerMoveComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	void (UPlayerMoveComponent::* MoveTriggeredFunc)(const FInputActionValue&) {};
	void (UPlayerMoveComponent::* MoveStartedFunc)(const FInputActionValue&) {};
	void (UPlayerMoveComponent::* MoveReleasedFunc)(const FInputActionValue&) {};

	TWeakObjectPtr<APlayerController> Controller{};
	TWeakObjectPtr<ACharacter> Player{};

	bool bIsMoveable{ false };
public:	
	UPlayerMoveComponent();

protected:
	virtual void BeginPlay() override;

	bool CheckFunction(void (UPlayerMoveComponent::* Func)(const FInputActionValue&), const FString& FunctionName = "") const;

public:	
	void MoveStarted(const FInputActionValue& Value);
	void MoveTriggered(const FInputActionValue& Value);
	void MoveReleased(const FInputActionValue& Value);

	void SetMoveable(bool bInIsMoveable) { bIsMoveable = bInIsMoveable; }
	bool IsMoveable() const { return bIsMoveable; }
protected:
	void MoveDefault(const FInputActionValue& Value);

};
