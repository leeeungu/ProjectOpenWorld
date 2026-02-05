#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/Interface/InventorySlotInterface.h"
#include "InventoryComponent.generated.h"

//class UItemPrimaryDataAsset;
class ABasePlayer;

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
	float* maxInventoryWeight;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemData")
	int inventoryRow = 7;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemData")
	int inventoryCol = 6;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	int inventorySize{};

	TSoftObjectPtr<ABasePlayer> PlayerCharacter{};
public:
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnUpdateInventory	onUpdateInventory{};
public:	
	UInventoryComponent();
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(FName NewItemID, int ItemCount = 1);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasItem(FName SearchItemID, int SearchItemCount = 1) const;
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(FName RemoveItemID, int RemoveItemCount = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool DeleteItem(int Row, int Col);
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int GetItemCount(FName SearchItemID) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UseItem(int Row, int Col);

	//UFUNCTION(BlueprintCallable, Category = "Inventory")
	//bool SetInevntorySlot(int Row, int Col, FName NewItemID, int ItemCount = 1);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SwapSlot(int SrcRow, int SrcCol, int DstRow, int DstCol);

	bool GetInventorySlotData(int Row, int Col, const  FInventorySlot*& SlotData);
	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE bool GetSlotData (int Row, int Col,  FInventorySlot& SlotData) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE int GetInventoryRow() const { return inventoryRow; }
	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE int GetInventoryCol() const { return inventoryCol; }
	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE float GetInventoryWeight() const { return totalInventoryWeight; }
	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE float GetInventoryMaxWeight() const { return maxInventoryWeight ? *maxInventoryWeight : 1.0f; }
protected:
	virtual void BeginPlay() override;
};
