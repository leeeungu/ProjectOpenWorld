#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Pal/Interface/PalHarvestable.h"
#include "PalHarvestSMComponent.generated.h"

class IPalWorkerInterface;

UCLASS()
class PROJECTOPENWORLD_API UPalHarvestSMComponent : public UStaticMeshComponent, public IPalHarvestable
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Harvest")
	int HarvestMaxCount = 1;
	UPROPERTY(VisibleAnywhere, Category = "Harvest")
	int HarvestCurCoun{};

	TSet<TScriptInterface<IPalWorkerInterface>> Workers{};

	UFUNCTION()
	void Respawn();
public:
	virtual void OnHarvestEvent(FHarvestEventData EventData) override;

	void ResiterWorker(TScriptInterface<IPalWorkerInterface> WorkerClass);
	void UnregisterWorker(TScriptInterface<IPalWorkerInterface> WorkerClass)  ;
	bool IsWorkable() const ;
};
