#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InputActionValue.h"
#include "PlayerInputInterface.generated.h"

UENUM(BlueprintType)
enum class EInputKeyType : uint8
{
	None        UMETA(DisplayName = "None"),
	WASD		UMETA(DisplayName = "WASD"),
	SpaceBar    UMETA(DisplayName = "SpaceBar"),
	MouseAxis    UMETA(DisplayName = "MouseAxis"),
	KeyF		UMETA(DisplayName = "KeyF"),
	Esc         UMETA(DisplayName = "Esc"),
	KeyC        UMETA(DisplayName = "KeyC"),
	MouseR      UMETA(DisplayName = "MouseR"),
	MouseL      UMETA(DisplayName = "MouseL"),
	MouseWheel  UMETA(DisplayName = "MouseWheel"),
	KeyB   UMETA(DisplayName = "KeyB"),
};

UINTERFACE(MinimalAPI)
class UPlayerInputInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IPlayerInputInterface
{
	GENERATED_BODY()
public:
	virtual void StartEvent(const FInputActionValue& Value, EInputKeyType KeyType) = 0;
	virtual void TriggerEvent(const FInputActionValue& Value, EInputKeyType KeyType) = 0;
	virtual void CompleteEvent(const FInputActionValue& Value, EInputKeyType KeyType) = 0;

	//virtual void InputEvent(const FInputActionValue& Value) = 0;
};

UINTERFACE(MinimalAPI)
class UPlayerInputSettingInterface : public UInterface
{
	GENERATED_BODY()

};
class PROJECTOPENWORLD_API IPlayerInputSettingInterface
{
	GENERATED_BODY()
public:
	virtual void SetInputInterface(EInputKeyType KeyType, TScriptInterface<IPlayerInputInterface> InputInterface) = 0;
	virtual void ResetDeaflut(EInputKeyType KeyType) = 0;
	//virtual void InputEvent(const FInputActionValue& Value) = 0;
};
