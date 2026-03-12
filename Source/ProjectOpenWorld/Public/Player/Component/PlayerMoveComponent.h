#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/Interface/PlayerInputInterface.h"
#include "PlayerMoveComponent.generated.h"

struct FInputActionValue;
class APlayerController;
class ABasePlayer;
enum class EInputKeyType : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPlayerMoveComponent : public UActorComponent, public IPlayerInputInterface
{
	GENERATED_BODY()
private:
	void (UPlayerMoveComponent::* MoveTriggeredFunc)(const FInputActionValue&) {};
	void (UPlayerMoveComponent::* MoveStartedFunc)(const FInputActionValue&) {};
	void (UPlayerMoveComponent::* MoveReleasedFunc)(const FInputActionValue&) {};

	TWeakObjectPtr<APlayerController> Controller{};
	TWeakObjectPtr<ABasePlayer> Player{};

	EInputKeyType CurrentKeyType{};
	bool bIsMoveable{ false };
public:	
	UPlayerMoveComponent();

protected:
	virtual void BeginPlay() override;

	bool CheckFunction(void (UPlayerMoveComponent::* Func)(const FInputActionValue&), const FString& FunctionName = "") const;

public:	
	// 	IPlayerInputInterface implementation
	virtual void StartEvent(const FInputActionValue& Value, EInputKeyType KeyType) override;
	virtual void TriggerEvent(const FInputActionValue& Value, EInputKeyType KeyType) override;
	virtual void CompleteEvent(const FInputActionValue& Value, EInputKeyType KeyType) override;

	//void MoveStarted(const FInputActionValue& Value);
	//void MoveTriggered(const FInputActionValue& Value);
	//void MoveReleased(const FInputActionValue& Value);

	void SetMoveable(bool bInIsMoveable) { bIsMoveable = bInIsMoveable; }
	bool IsMoveable() const { return bIsMoveable; }

	void SetTopDownMode();
	void SetDefaultMove();
	void SetSwordMove();

protected:
	void TriggerDefault(const FInputActionValue& Value);
	void StartDefault(const FInputActionValue& Value);
	void CompleteDefault(const FInputActionValue& Value);

	void TriggerSword(const FInputActionValue& Value);

	void TriggerTopDownMode(const FInputActionValue& Value);

};
