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
	Key1	,
	Key2	,
	Key3	,
	KeyE	,
	KeyTab	,
	InputMax,
};

class UInputAction;

USTRUCT(BlueprintType)
struct FPlayerInputData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerInputData")
	TObjectPtr<UInputAction> InputAction{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerInputData")
	EInputKeyType InputKeyType{ EInputKeyType::None };

};


UINTERFACE(MinimalAPI)
class UPlayerInputInterface : public UInterface
{
	GENERATED_BODY()
};

/*
* 인터페이스 구현된 객체로 넘겨서 
* 책임이 확실하고 수정에 용이함
  (키 입력 시 여러 상태 채크를 안해도 되서 코드의 복잡도가 줄어들고 성능 향상이 있다.)
* 결합도가 높아지는 경향이 있음 (ex buildingassist)
*/
//
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
