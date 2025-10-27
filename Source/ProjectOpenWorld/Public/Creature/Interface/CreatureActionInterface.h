#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CreatureActionInterface.generated.h"

//class AActor;
//

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ECreateRollType : uint8
{
	Roll_None = 0,
	Roll_Buliding = 1,
	Roll_Mining = 2,
	Roll_Crafting =4,
	Roll_Attack = 8,
};


UENUM(Blueprintable)
enum class ECreateActionType  : uint8
{
	Action_None,
	Action_Buliding,
	Action_Mining,
	Action_Crafting,
	Action_Attack,
	Action_Max
};

UINTERFACE(MinimalAPI, BlueprintType)
class UCreatureActionInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API ICreatureActionInterface
{
	GENERATED_BODY()
public:
};
