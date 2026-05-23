#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/Interface/InventorySlotInterface.h"
#include "Item/DataTable/ItemSlotType.h"
#include "PalInventory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotChanged, int32, SlotIndex, const FInventorySlot&, NewSlot);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UPalInventory : public UActorComponent
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere)
	TArray<FInventorySlot> Slots{};
	UPROPERTY(EditAnywhere)
	int32 InventorySize{};
public:
	UPalInventory();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool FindSameItem(UBaseItem* NewItem, int32& Index) const;
	bool FindEmptySlot(int32& Index) const;
public:

	UPROPERTY(BlueprintAssignable)
	FOnSlotChanged OnSlotChanged;

	UFUNCTION(BlueprintCallable)
	bool AddItem(UBaseItem* NewItem);
	UFUNCTION(BlueprintCallable)
	bool RemoveItem(int32 SlotIndex, int32 ItemCount);
	UFUNCTION(BlueprintCallable)
	bool SwapSlots(int32 Src, int32 Dst);
	UFUNCTION(BlueprintPure)
	FInventorySlot GetInventorySlot(int32 Index) const;
	UFUNCTION(BlueprintPure)
	int32 GetInventorySize() const;

	UFUNCTION()
	void OnOpenUI();
	UFUNCTION()
	void OnSwapSlots(int32 Src, int32 Dst);
	UFUNCTION()
	void OnSlotUpdate(int32 Index);
	UFUNCTION()
	void OnRemoveItem(int32 Index);
	//UFUNCTION()
	//bool OnAddItemEvent(UBaseItem* ItemSlot);
};
