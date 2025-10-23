#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CreatureMessageInterface.generated.h"


class AActor;

UENUM(Blueprintable)
enum class EMessageType : uint8
{
	NONE,
	TakeRest,
	DoBuild,
	StopBuild,
	DoAttack,
	StopAttack,
	DoCraft,
	StopCraft,
};

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

	void ReceiveMessage(EMessageType MessageType, AActor* SendActor, UObject* TargetObject = nullptr);
};
