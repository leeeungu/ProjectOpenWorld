#pragma once

#include "CoreMinimal.h"
#include "Item/Object/BaseItemObject.h"
#include "ItemObject_Equip.generated.h"


class USkeletalMesh;

UCLASS()
class PROJECTOPENWORLD_API UItemObject_Equip : public UBaseItemObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData")
	TObjectPtr<USkeletalMesh> WeaponMesh = nullptr;

public:
	//UItemObject_Equip();

	virtual void RegisterItemObject(AActor* TargetActor) override;
	virtual void UnregisterItemObject(AActor* TargetActor) override;
};
