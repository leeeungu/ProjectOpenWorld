#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseItem.generated.h"

//enum class EItemUseType : uint8;
//class UItemAssetUserData;
class UAssetUserData;

UENUM(BlueprintType)
enum class EItemUseType : uint8
{
	None		UMETA(DisplayName = "None"),
	HandEquip	UMETA(DisplayName = "Hand Equip"),
	ArmorEquip		UMETA(DisplayName = "Armor Equip"),
	Consume		UMETA(DisplayName = "Consume"),
	Throw		UMETA(DisplayName = "Throw"),
	UseTypeEnumMax UMETA(Hidden)
	//Place		UMETA(DisplayName = "Place")
};

UCLASS(Blueprintable)
class PROJECTOPENWORLD_API UBaseItem : public UObject //, public IInterface_AssetUserData
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere, Category = "Item", meta = (AllowPrivateAccess = "true", BlueprintGetter = GetUseType, BlueprintSetter = SetUseType))
	EItemUseType UseType{};
	UPROPERTY(VisibleAnywhere, Category = "Item", meta = (AllowPrivateAccess = "true", BlueprintGetter = GetItemID, BlueprintSetter = SetItemID))
	FName ItemID{};
	UPROPERTY(VisibleAnywhere, Category = "Item", meta = (AllowPrivateAccess = "true", ClampMin = 0))
	int32 ItemCount{};

public:
	FORCEINLINE void SetUseType(EItemUseType InUseType) { UseType = InUseType; }
	FORCEINLINE EItemUseType GetUseType() const { return UseType; }	
	FORCEINLINE void SetItemID(FName InItemID) { ItemID = InItemID; }
	FORCEINLINE FName GetItemID() const { return ItemID; }
	FORCEINLINE void SetItemCount(int32 InItemCount) { ItemCount = FMath::Max(0, InItemCount); }
	FORCEINLINE int32 GetItemCount() const { return ItemCount; }
};