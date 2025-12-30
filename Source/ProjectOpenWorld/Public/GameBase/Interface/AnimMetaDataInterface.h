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
class UAnimMetaDatLoopInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IAnimMetaDatLoopInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimMetaData")
	void InitLoopMetaData(ACharacter* pOwner, float Start, float Loop);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimMetaData")
	void LoopUpdate(float DeltaTime);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimMetaData")
	bool IsLoopingEnd() const;
};


UINTERFACE(MinimalAPI)
class UAnimMetaDataMovementInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IAnimMetaDataMovementInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimMetaData")
	void InitUpdateMetaData(ACharacter* pOwner, float StartLength, float LoopLength, float EndLength);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimMetaData")
	void UpdateMetaData(float DeltaTime);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimMetaData")
	bool IsMoveEnd() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimMetaData")
	bool IsLoopingEnd() const;
};
