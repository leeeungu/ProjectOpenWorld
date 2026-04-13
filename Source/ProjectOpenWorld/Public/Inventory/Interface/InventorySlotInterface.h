#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventorySlotInterface.generated.h"

//class UItemPrimaryDataAsset;
class UBaseItem;

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()
public:
	FInventorySlot() = default;

	explicit FInventorySlot(FName NewItemID)
		: ItemID(NewItemID)
		, isEmpthySlot(NewItemID.IsNone())
	{
	}

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	TObjectPtr<UBaseItem> ItemObject{};

	// 기존 UI/블루프린트 호환용 캐시
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	FName ItemID{ NAME_None };

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	int32 ItemCount{ 0 };
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	int32 ItemSeed{};

	//UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	//float ItemTotalWeights{ 0.f };

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	bool isEmpthySlot = true;

	bool operator==(const FInventorySlot& sDst) const;

	void RefreshFromObject();
	

	void Clear()
	{
		ItemObject = nullptr;
		ItemID = NAME_None;
		ItemCount = 0;
		//ItemTotalWeights = 0.f;
		isEmpthySlot = true;
	}
};
//USTRUCT(BlueprintType)
//struct FInventorySlot
//{
//	GENERATED_BODY()
//public:
//	FInventorySlot() = default;
//
//	FInventorySlot(FName NewItemID);
//	//FInventorySlot(UItemPrimaryDataAsset* DataAsset);
//
//
//	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
//	FName ItemID{};
//	//UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
//	//TObjectPtr<UItemPrimaryDataAsset> ItemDataAsset{};
//	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
//	int ItemCount{};
//	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
//	float ItemTotalWeights{};
//
//	bool isEmpthySlot = true;
//
//	bool operator==(const FInventorySlot& sDst);
//};

UINTERFACE(MinimalAPI)
class UInventorySlotInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IInventorySlotInterface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void SetSlotData(const FInventorySlot& Data);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void SetSlotIndex(int Row, int Col);
};
