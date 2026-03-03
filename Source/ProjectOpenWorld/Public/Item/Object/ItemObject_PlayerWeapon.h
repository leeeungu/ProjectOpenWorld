#pragma once

#include "CoreMinimal.h"
#include "Item/Object/BaseItemObject.h"
#include "ItemObject_PlayerWeapon.generated.h"

class USkeletalMesh;
enum class EPlayerState : uint8;

UCLASS()
class PROJECTOPENWORLD_API UItemObject_PlayerWeapon : public UBaseItemObject
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData")
	TObjectPtr<USkeletalMesh> WeaponMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData")
	FName WeaponSocketName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData")
	EPlayerState EquipPlayerState{};
public:
	UItemObject_PlayerWeapon();

	virtual void RegisterItemObject(AActor* TargetActor) override;
	virtual void UnregisterItemObject(AActor* TargetActor) override;
};
