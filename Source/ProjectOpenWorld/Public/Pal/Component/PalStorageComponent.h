#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Interface/PalStoreInterface.h"
#include "Pal/Interface/PalSaveGameObject.h"
#include "PalStorageComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPalStoreChanged, int, ChangeIndex, AActor*, ChangedPal);

USTRUCT(BlueprintType)
struct FPalStoreInventoryData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalStoreData")
	TObjectPtr<AActor> SpawnActor{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalStoreData")
	int32 Index = -1;
};

USTRUCT()
struct FPalStorageSaveData
{
	GENERATED_BODY()
	UPROPERTY() int32          SlotIndex = -1;
	UPROPERTY() FSoftClassPath PalClass {};
	UPROPERTY() TArray<uint8>  ByteData{};
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalStorageComponent : public UActorComponent, public IPalSaveGameObject
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "PalStore")
	TArray< TObjectPtr<AActor>> PalStorage{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PalStore")
	int InventorySize = 30;

	int CurrnetInventorySize = 0;
public:	
	UPalStorageComponent();

	FOnPalStoreChanged OnPalStoreChanged{};
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

	UFUNCTION(BlueprintPure, Category = "PalStore")
	AActor* GetStoredPal(int InventoryIndex) const;

	void SwapStoredPals(int IndexA, int IndexB);
	int GetInventorySize() const {return InventorySize;}
	//void ShowAllSpawnedPals();
	//void HideAllSpawnedPals();
protected:
	UPROPERTY(SaveGame)
	TArray<FPalStorageSaveData> SavedStorage{};
public:
	virtual void OnPreSave() override;
	virtual void OnLoaded() override;
};
