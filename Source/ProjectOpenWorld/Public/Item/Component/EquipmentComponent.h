#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"

class UBaseItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEquipmentComponent();

protected:
	virtual void BeginPlay() override;

public:
	//UFUNCTION(BlueprintCallable, Category = "Equipment")
	virtual bool EquipItem(UBaseItem* Item) PURE_VIRTUAL(UEquipmentComponent::EquipItem, return false;);
	virtual bool UnequipItem(UBaseItem* Item) PURE_VIRTUAL(UEquipmentComponent::UnequipItem	, return false;);
};
