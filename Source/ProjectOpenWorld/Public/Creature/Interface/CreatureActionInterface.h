#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CreatureActionInterface.generated.h"

class AActor;

//UINTERFACE(MinimalAPI)
//class UCreatureActionInterface : public UInterface
//{
//	GENERATED_BODY()
//};
//
//class PROJECTOPENWORLD_API ICreatureActionInterface
//{
//	GENERATED_BODY()
//public:
//	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CreatureMessage")
//};


UINTERFACE(MinimalAPI)
class UBuildingActionInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IBuildingActionInterface
{
	GENERATED_BODY()
public:
};


UINTERFACE(MinimalAPI)
class UCraftActionInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API ICraftActionInterface
{
	GENERATED_BODY()
public:
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CreatureMessage")
	//
	//void ReceiveMessage(EMessageType MessageType, AActor* SendActor);
};


UINTERFACE(MinimalAPI)
class UAttackActionInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IAttackActionInterface
{
	GENERATED_BODY()
public:
};

