#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PalWorkerInterface.generated.h"

UINTERFACE(MinimalAPI, Meta = (CannotImplementInterfaceInBlueprint))
class UPalWorkerInterface : public UInterface
{
	GENERATED_BODY()
};

class IPalWorkerInterface
{
	GENERATED_BODY()
public:
	UFUNCTION( BlueprintCallable, Category = "Pal|Worker")
	virtual void StartWorking() {}
	UFUNCTION(BlueprintCallable,Category = "Pal|Worker")
	virtual void StopWorking() {}
	UFUNCTION(BlueprintCallable, Category = "Pal|Worker")
	virtual void EndWorking(bool bSuccess) {}
};
