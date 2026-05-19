#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PalWorkable.generated.h"

class IPalWorkerInterface;
class UPalWorkerInterface;

UINTERFACE(MinimalAPI, Meta = (CannotImplementInterfaceInBlueprint))
class UPalWorkable : public UInterface
{
	GENERATED_BODY()
};

class IPalWorkable
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Pal|Workable")
	virtual void ResiterWorker(TScriptInterface<IPalWorkerInterface> WorkerClass) {}
	UFUNCTION(BlueprintCallable, Category = "Pal|Workable")
	virtual void UnregisterWorker(TScriptInterface<IPalWorkerInterface> WorkerClass) {}
	UFUNCTION(BlueprintCallable, Category = "Pal|Workable")
	virtual bool IsWorkable() const { return false; }
};
