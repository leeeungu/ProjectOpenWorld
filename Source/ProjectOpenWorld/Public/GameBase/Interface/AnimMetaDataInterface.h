#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AnimMetaDataInterface.generated.h"

UINTERFACE()
class UAnimMetaDataInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IAnimMetaDataInterface
{
	GENERATED_BODY()
public:
};

class ACharacter;

UINTERFACE(MinimalAPI)
class UAnimMetaDataStartInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IAnimMetaDataStartInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimMetaData")
	void StartMetaData(ACharacter* pOwner);
};


UINTERFACE(MinimalAPI)
class UAnimMetaDataUpdateInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IAnimMetaDataUpdateInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimMetaData")
	void InitUpdateMetaData(ACharacter* pOwner, float UpdateLength);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimMetaData")
	void UpdateMetaData(float DeltaTime);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimMetaData")
	bool IsLoopingEnd() const;
};
