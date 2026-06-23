#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Pal/Data/PalJobTypes.h"
#include "PalWorkable.generated.h"

class IPalWorkerInterface;
class UPalWorkerInterface;
class UActorComponent;

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
	UFUNCTION(BlueprintCallable, Category = "Pal|Workable")
	virtual EPalJobType GetWorkJobType() const { return EPalJobType::None; }
	UFUNCTION(BlueprintCallable, Category = "Pal|Workable")
	virtual  FPalWorkCommand GetWorkCommand(AActor* InstigatorActor, AActor* Target) const { return FPalWorkCommand{}; }
	virtual  FPalWorkCommand GetWorkCommandComponent(AActor* InstigatorActor, UActorComponent* Target, int32 Index) const { return FPalWorkCommand{}; }
	
};
