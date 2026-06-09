#pragma once

#include "CoreMinimal.h"
#include "Item/Component/EquipmentComponent.h"
#include "Player/Interface/PlayerInputInterface.h"
#include "PlayerEquipComponent.generated.h"

enum class EWeapone : uint8;
class UBaseItem;
class UHandEquipItemFragment;
class UWeaponeAssetUserData;
class UItemDataAsset;
struct FInventorySlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquipRegister, const UBaseItem*, NewEquip, const UBaseItem*, OldEquip);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquipChanged, EWeapone, Type, const UBaseItem*, Equip);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPlayerEquipComponent : public UEquipmentComponent, public IPlayerInputInterface
{
	GENERATED_BODY()
private:
	EWeapone CurrentWeapone{};
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerEquip")
	TMap<EWeapone, const UBaseItem*> EquipItemMap{};

	UPROPERTY()
	TObjectPtr< USkeletalMeshComponent> PlayerMesh{};
	UPROPERTY()
	TObjectPtr< USkeletalMeshComponent> WeaponMesh{};

	const UBaseItem* CurrentEquipItem{};
public:	
	UPlayerEquipComponent();

	UPROPERTY(BlueprintAssignable, Category = "PlayerEquip")
	FOnEquipChanged OnEquipChanged{};

protected:
	virtual void BeginPlay() override;

public:	
	virtual bool EquipItem(const UBaseItem* Item) override;
	virtual bool UnequipItem(const UBaseItem* Item) override;
	virtual bool RegisterItem(const UBaseItem* Item) override;
	virtual bool UnRegisterItem(const UBaseItem * Item) override;

	bool IsEquipSlot(const UBaseItem* Item) const;
	bool UnEquipCurrent() ;
	bool IsEquip() const;

	// 	IPlayerInputInterface implementation
	virtual void StartEvent(const FInputActionValue& Value, EInputKeyType KeyType) override;
	virtual void TriggerEvent(const FInputActionValue& Value, EInputKeyType KeyType) override;
	virtual void CompleteEvent(const FInputActionValue& Value, EInputKeyType KeyType) override;


	UFUNCTION()
	void OnUpdateEquip(const TArray< FInventorySlot>& Data);
private:
	UHandEquipItemFragment* GetHandEquipFragment(const UBaseItem* Item) const;
	class UPlayerAnimationSLEDataFragment* GetPlayerAnimationSLEDataFragment(const UBaseItem* Item) const;
	UWeaponeAssetUserData* GetWeaponAssetUserData(USkeletalMesh* Mesh) const;
};
