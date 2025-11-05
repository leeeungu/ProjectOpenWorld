#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CreatureActionInterface.generated.h"

class AActor;

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ECreatureRollType : uint8
{
	Roll_None = 0,
	Roll_Buliding = 1,
	Roll_Mining = 2,
	Roll_Crafting =4,
	Roll_Attack = 8,
	Roll_Lift = 16,
};

UENUM(Blueprintable)
enum class ECreatureActionType  : uint8
{
	Action_None,
	Action_Move,
	Action_Buliding,
	Action_Resource,
	Action_Crafting,
	Action_Attack,
	Action_Transport,
	Action_Max
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FFinishAction, ECreatureActionType, ActionType, AActor*, SendActor, AActor*, TargetActor );

UINTERFACE(MinimalAPI, BlueprintType)
class UCreatureActionInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API ICreatureActionInterface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CreatureAction")
	bool ActionStart(AActor* SendActor, AActor* TargetActor = nullptr);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CreatureAction")
	bool ActionEnd();
};
