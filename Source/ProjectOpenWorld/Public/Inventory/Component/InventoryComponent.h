#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/Interface/InventorySlotInterface.h"
#include "InventoryComponent.generated.h"

class UItemPrimaryDataAsset;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateInventory);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	TArray<FInventorySlot> inventoryArray{};
	TArray<FInventorySlot*> inventoryViewArray{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	float totalInventoryWeight{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	float maxInventoryWeight = 300.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemData")
	int inventoryRow = 7;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemData")
	int inventoryCol = 6;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	int inventorySize{};
public:
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnUpdateInventory	onUpdateInventory{};
public:	
	UInventoryComponent();
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(UItemPrimaryDataAsset* ItemData, int ItemCount = 1);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SetInevntorySlot(int Row, int Col, UItemPrimaryDataAsset* ItemData, int ItemCount = 1);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SwapSlot(int SrcRow, int SrcCol, int DstRow, int DstCol);


	bool GetInventorySlotData(int Row, int Col, const  FInventorySlot*& SlotData);

	FORCEINLINE int GetInventoryRow() const { return inventoryRow; }
	FORCEINLINE int GetInventoryCol() const { return inventoryCol; }
	FORCEINLINE float GetInventoryWeight() const { return totalInventoryWeight; }
	FORCEINLINE float GetInventoryMaxWeight() const { return maxInventoryWeight; }
protected:
	virtual void BeginPlay() override;
		
};
