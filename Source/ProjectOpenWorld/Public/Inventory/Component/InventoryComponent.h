#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/Interface/InventorySlotInterface.h"
#include "InventoryComponent.generated.h"

class UItemPrimaryDataAsset;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	TArray<FInventorySlot> inventoryArray{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	float totalInventoryWeight{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	float maxInventoryWeight{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemData")
	int inventoryRow = 7;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemData")
	int inventoryCol = 6;
public:	
	UInventoryComponent();
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(UItemPrimaryDataAsset* ItemData);

	bool GetInventorySlotData(int Row, int Col, const  FInventorySlot* SlotData);

	FORCEINLINE int GetInventoryRow() const { return inventoryRow; }
	FORCEINLINE int GetInventoryCol() const { return inventoryCol; }
protected:
	virtual void BeginPlay() override;
		
};
