#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Item/Object/ItemDataFragment.h"
#include "ItemDataAsset.generated.h"

UENUM(BlueprintType)
enum class EItemUseType : uint8
{
	None		UMETA(DisplayName = "None"),
	HandEquip	UMETA(DisplayName = "Hand Equip"),
	ArmorEquip		UMETA(DisplayName = "Armor Equip"),
	Consume		UMETA(DisplayName = "Consume"),
	Throw		UMETA(DisplayName = "Throw"),
	UseTypeEnumMax UMETA(Hidden)
};

UCLASS()
class PROJECTOPENWORLD_API UItemDataAsset : public UDataAsset
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, Category = "ItemData")
	EItemUseType ItemUseType = EItemUseType::None;
	UPROPERTY(EditInstanceOnly, Instanced, Category = "ItemData")
	TArray<TObjectPtr< UItemDataFragment>> ItemDataFragments{};

public:
	const TArray<TObjectPtr<UItemDataFragment>> GetItemDataFragmentArrayOfClass(TSubclassOf<UItemDataFragment> InUserDataClass);
	const TObjectPtr<UItemDataFragment> GetItemDataFragmentOfClass(TSubclassOf<UItemDataFragment> InUserDataClass);
	const TArray<TObjectPtr<UItemDataFragment>>* GetItemDataFragmentArray();
	EItemUseType GetItemUseType() const { return ItemUseType; }
};
