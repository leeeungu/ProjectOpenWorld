#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseItem.generated.h"

class UAssetUserData;
enum class EItemUseType : uint8;

UCLASS(Blueprintable)
class PROJECTOPENWORLD_API UBaseItem : public UObject //, public IInterface_AssetUserData
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere, Category = "Item", meta = (AllowPrivateAccess = "true", BlueprintGetter = GetItemID, BlueprintSetter = SetItemID))
	FName ItemID{};
	UPROPERTY(VisibleAnywhere, Category = "Item", meta = (AllowPrivateAccess = "true", ClampMin = 0))
	int32 ItemCount{};

public:
	FORCEINLINE EItemUseType GetUseType() const;
	FORCEINLINE void SetItemID(FName InItemID) { ItemID = InItemID; }
	FORCEINLINE FName GetItemID() const { return ItemID; }
	FORCEINLINE void SetItemCount(int32 InItemCount) { ItemCount = FMath::Max(0, InItemCount); }
	FORCEINLINE int32 GetItemCount() const { return ItemCount; }
};