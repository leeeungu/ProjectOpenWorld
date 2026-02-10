#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Interface/PalStoreInterface.h"
#include "PalStorageComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPalStoreChanged, int, ChangeIndex, AActor*, ChangedPal);

USTRUCT(BlueprintType)
struct FPalStoreInventoryData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalStoreData")
	TSubclassOf<AActor> SpawnActorClass{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalStoreData")
	bool bSpawned{};
	//FPalStoredData Data{};

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalStorageComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "PalStore")
	TArray< TObjectPtr<AActor>> PalStorage{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "PalStore")
	TArray< TObjectPtr<AActor>> SpawnedPal{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PalStore")
	int InventorySize = 30;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PalStore")
	int SpawnSize = 5;

	int CurrnetInventorySize = 0;
	int CurrentSpawnedSize = 0;
public:	
	UPalStorageComponent();
	FOnPalStoreChanged OnPalStoreChanged{};
	FOnPalStoreChanged OnPalSpawnChanged{};
protected:
	virtual void BeginPlay() override;


	UFUNCTION()
	void PalDead(AActor* DeadPal);
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category = "PalStore")
	bool CanStorePal() const;

	UFUNCTION(BlueprintCallable, Category = "PalStore")
	bool StorePal(FPalStoreInventoryData NewPal);
	UFUNCTION(BlueprintCallable, Category = "PalStore")
	void RemovePal(int InventoryIndex);

	UFUNCTION(BlueprintCallable, Category = "PalStore")
	AActor* SpawnPal(int InventoryIndex, int SpawnIndex);
	UFUNCTION(BlueprintCallable, Category = "PalStore")
	void DeSpawnPal(int InventoryIndex, int SpawnIndex);

	UFUNCTION(BlueprintPure, Category = "PalStore")
	AActor* GetStoredPal(int InventoryIndex) const;

	void SwapStoredPals(int IndexA, int IndexB);
	void SwapSpawnedPals(int IndexA, int IndexB);
	int32 GetSpawnInventorySize() const { return SpawnSize; }
	void ShowAllSpawnedPals();
	void HideAllSpawnedPals();
};
