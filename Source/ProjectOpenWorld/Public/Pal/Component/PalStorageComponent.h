#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Interface/PalStoreInterface.h"
#include "PalStorageComponent.generated.h"


USTRUCT(BlueprintType)
struct FPalStoreInventoryData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalStoreData")
	TSubclassOf<AActor> SpawnClass{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalStoreData")
	bool bSpawned{};
	FPalStoredData Data{};

};


USTRUCT(BlueprintType)
struct FPalStorageData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalStoreData")
	TSubclassOf<AActor> SpawnClass{};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalStorageComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "PalStore")
	TArray<FPalStoreInventoryData> PalStorage{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "PalStore")
	TMap< TObjectPtr<AActor>, int> SpawnedPal{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PalStore")
	int InventorySize = 30;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PalStore")
	int SpawnSize = 30;
public:	
	UPalStorageComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category = "PalStore")
	bool CanStorePal(int Index) const;

	UFUNCTION(BlueprintCallable, Category = "PalStore")
	void StorePal(FPalStoreInventoryData NewPal, int Index);
	UFUNCTION(BlueprintCallable, Category = "PalStore")
	void RemovePal(FPalStoreInventoryData  TargetPal, int Index);

	UFUNCTION(BlueprintCallable, Category = "PalStore")
	AActor* SpawnPal(int Index);
	UFUNCTION(BlueprintCallable, Category = "PalStore")
	void DeSpawnPal(AActor* TargetPal);

};
