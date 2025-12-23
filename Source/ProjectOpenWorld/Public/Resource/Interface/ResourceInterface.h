#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ResourceInterface.generated.h"

class AResourceActor;

UINTERFACE(MinimalAPI)
class UResourceInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IResourceInterface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Resource")
	float GetResourceSpeed() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Resource")
	void StartResource(AResourceActor* ResourceActor);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Resource")
	void StopResource(AResourceActor* ResourceActor);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Resource")
	void EndResource(AResourceActor* ResourceActor);
};
