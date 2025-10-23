#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CreatureMessageInterface.generated.h"

UENUM()
enum class EMessageType :uint8
{

};

UINTERFACE(MinimalAPI)
class UCreatureMessageInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API ICreatureMessageInterface
{
	GENERATED_BODY()
public:

};
