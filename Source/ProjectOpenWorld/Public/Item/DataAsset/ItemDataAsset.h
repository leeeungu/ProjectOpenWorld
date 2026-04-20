#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Item/Object/ItemDataFragment.h"
#include "Item/DataTable/ItemUseType.h"
#include "ItemDataAsset.generated.h"

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
