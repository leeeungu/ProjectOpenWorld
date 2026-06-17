#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Pal/Data/PalJobTypes.h"
#include "PalWorkerInterface.generated.h"

class UPalJobComponent;

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
	UFUNCTION(BlueprintCallable, Category = "Pal|Worker")
	virtual float GetWorkSpeed(EPalJobType JobType) { return 1.0f; }
	virtual bool IsCanWork() const { return true; }
	virtual UPalJobComponent* GetPalJobComponent() const { return nullptr; }
	
};
