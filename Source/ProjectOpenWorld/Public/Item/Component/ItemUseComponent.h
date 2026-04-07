#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemUseComponent.generated.h"

class UEquipmentComponent;
class UBaseItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UItemUseComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	bool (UItemUseComponent::* UseHandler[7])(UBaseItem*) {};

	//UPROPERTY()
	TObjectPtr<	UEquipmentComponent> EquipmentComponent{};
public:	
	UItemUseComponent();

	UFUNCTION(BlueprintCallable, Category = "Item")
	bool UseItem(UBaseItem* Item);
protected:
	virtual void BeginPlay() override;

	bool HandleDefault(UBaseItem* Item);
	bool HandleHandEquip(UBaseItem* Item);
	bool HandleArmorEquip(UBaseItem* Item);
	bool HandleConsume(UBaseItem* Item);
	bool HandleThrow(UBaseItem* Item);
};