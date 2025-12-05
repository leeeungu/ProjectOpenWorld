#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PalStorageComponent.generated.h"


USTRUCT(BlueprintType)
struct FPalStorageData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalStoreData")
	bool bSpawned{};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalStorageComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TMap<TObjectPtr<AActor>, FPalStorageData> PalStorage{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int InventorySize = 20;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int SpawnSize = 5;
public:	
	UPalStorageComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure)
	bool CanStorePal() const { return true; }

	UFUNCTION(BlueprintCallable)
	void StorePal(AActor* NewPal);

	UFUNCTION(BlueprintCallable)
	void SpawnPal(AActor* TargetPal) {}
		
};
