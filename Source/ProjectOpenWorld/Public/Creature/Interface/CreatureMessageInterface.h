#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CreatureMessageInterface.generated.h"


class AActor;

UENUM(Blueprintable)
enum class EMessageType : uint8
{
	NONE,
};

enum class ECreatureActionType :uint8;

UINTERFACE(MinimalAPI, BlueprintType)
class UCreatureMessageInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API ICreatureMessageInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CreatureMessage")

	void ReceiveMessage(EMessageType MessageType, AActor* SendActor, AActor* TargetObject = nullptr);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CreatureAction")

	void ReceiveActionMessage(ECreatureActionType MessageType, AActor* SendActor, AActor* TargetObject = nullptr);
};
