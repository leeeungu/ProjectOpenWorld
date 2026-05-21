#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pal/Interface/PalWorkable.h"
#include "PalHarvestActor.generated.h"

class UPalHarvestSMComponent;
class IPalWorkerInterface;

UCLASS(Blueprintable)
class PROJECTOPENWORLD_API APalHarvestActor : public AActor, public IPalWorkable
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, Category = "Harvest")
	TObjectPtr<UPalHarvestSMComponent> MeshComponent{};

public:	
	APalHarvestActor();

public:
	virtual void ResiterWorker(TScriptInterface<IPalWorkerInterface> WorkerClass)  override;
	virtual void UnregisterWorker(TScriptInterface<IPalWorkerInterface> WorkerClass)  override;
	virtual bool IsWorkable() const override;
	virtual EPalJobType GetWorkJobType() const { return EPalJobType::Mining; }
	virtual FPalWorkCommand GetWorkCommand(AActor* InstigatorActor, AActor* Target) const override;
};
