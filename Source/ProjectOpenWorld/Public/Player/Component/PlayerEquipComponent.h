#pragma once

#include "CoreMinimal.h"
#include "Item/Component/EquipmentComponent.h"
#include "Player/Interface/PlayerInputInterface.h"
#include "PlayerEquipComponent.generated.h"

enum class EWeapone : uint8;
class UBaseItem;
class UHandEquipItemFragment;
class UWeaponeAssetUserData;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPlayerEquipComponent : public UEquipmentComponent, public IPlayerInputInterface
{
	GENERATED_BODY()
private:
	EWeapone CurrentWeapone{};
protected:
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerEquip")
	//TMap<EWeapone, FString> EquipSocket{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerEquip")
	TMap<EWeapone, TObjectPtr<UBaseItem>> EquipItemMap{};

	UPROPERTY()
	TObjectPtr< USkeletalMeshComponent> PlayerMesh{};
	UPROPERTY()
	TObjectPtr< USkeletalMeshComponent> WeaponMesh{};

	TObjectPtr< UBaseItem> CurrentEquipItem{};
public:	
	UPlayerEquipComponent();

protected:
	virtual void BeginPlay() override;

public:	
	//void SetWeapone(EWeapone NewWeapone) { CurrentWeapone = NewWeapone; }

	virtual bool EquipItem(UBaseItem* Item) override;
	virtual bool UnequipItem(UBaseItem* Item) override;

	// 	IPlayerInputInterface implementation
	virtual void StartEvent(const FInputActionValue& Value, EInputKeyType KeyType) override;
	virtual void TriggerEvent(const FInputActionValue& Value, EInputKeyType KeyType) override;
	virtual void CompleteEvent(const FInputActionValue& Value, EInputKeyType KeyType) override;

private:
	UHandEquipItemFragment* GetHandEquipFragment(UBaseItem* Item);
	class UPlayerAnimationSLEDataFragment* GetPlayerAnimationSLEDataFragment(UBaseItem* Item);
	UWeaponeAssetUserData* GetWeaponAssetUserData(USkeletalMesh* Mesh);
	
	//void SetEquipMesh(USkeletalMesh* NewMesh);
	//void SetUnequipMesh(USkeletalMesh* OldMesh);
};
//namespace
//{
//	UHandEquipItemFragment* GetHandEquipFragment(const UBaseItem* Item)
//	{
//		if (!Item || !UItemDataSubsystem::IsValidInstance())
//			return nullptr;
//
//		UItemDataAsset* ItemDataAsset = UItemDataSubsystem::GetPalItemDataAssetByName(Item->GetItemID());
//		if (!ItemDataAsset)
//			return nullptr;
//
//		const TArray<TObjectPtr<UItemDataFragment>> Fragments =
//			ItemDataAsset->GetItemDataFragmentOfClass(UHandEquipItemFragment::StaticClass());
//
//		if (Fragments.IsEmpty())
//			return nullptr;
//
//		return Cast<UHandEquipItemFragment>(Fragments[0]);
//	}
//
//	UWeaponeAssetUserData* GetWeaponAssetUserData(USkeletalMesh* Mesh)
//	{
//		if (!Mesh)
//			return nullptr;
//
//		return Cast<UWeaponeAssetUserData>(
//			Mesh->GetAssetUserDataOfClass(UWeaponeAssetUserData::StaticClass()));
//	}
//}