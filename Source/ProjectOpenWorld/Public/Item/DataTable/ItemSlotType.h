#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemSlotType.generated.h"

UENUM()
enum class EItemSlotType : uint8
{
	None,
	Weapon,
	Axe,
	PickAxe,
	HeadEquip,
	BodyEquip,
	Shiled,
	Glider,
	SlotTypeEnumMax UMETA(Hidden)
};

class UBaseItem;

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()
public:
	FInventorySlot() = default;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData", SaveGame)
	TObjectPtr<UBaseItem> ItemObject{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	int32 SlotIndex = -1;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Inventory")
	EItemSlotType SlotType{};

	bool operator==(const FInventorySlot& sDst) const;
	FName GetItemID() const;
	int32 GetItemCount() const;
	void Clear();

	bool Swap(FInventorySlot* Dst);
};

USTRUCT()
struct FPalSlotSaveData
{
	GENERATED_BODY()
public:
	UPROPERTY(SaveGame) 
	int32 SlotIndex = -1;
	UPROPERTY(SaveGame) 
	FName ItemID = NAME_None;
	UPROPERTY(SaveGame) 
	int32 Count = 0;
	UPROPERTY(SaveGame) 
	uint8 SlotType = 0;   // EItemSlotType
};