#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Item/Object/ItemDataFragment.h"
#include "ItemDataAsset.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UItemDataAsset : public UDataAsset
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditInstanceOnly, Instanced, Category = "ItemData")
	TArray<TObjectPtr< UItemDataFragment>> ItemDataFragments{};
	//TArray<TObjectPtr<UAssetUserData>> AssetUserData;
public:
	// IInterface_AssetUserData interface
	const TArray<TObjectPtr<UItemDataFragment>> GetItemDataFragmentOfClass(TSubclassOf<UItemDataFragment> InUserDataClass);
	const TArray<TObjectPtr<UItemDataFragment>>* GetItemDataFragmentArray();
};
