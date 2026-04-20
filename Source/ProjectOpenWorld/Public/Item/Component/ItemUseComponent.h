#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/DataTable/ItemUseType.h"
#include "ItemUseComponent.generated.h"

class UBaseItem;
class ABasePlayer;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UItemUseComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	bool (UItemUseComponent::* UseHandler[static_cast<uint8>(EItemUseType::UseTypeEnumMax)])(UBaseItem*) {};
	bool (UItemUseComponent::* UnUseHandler[static_cast<uint8>(EItemUseType::UseTypeEnumMax)])(UBaseItem*) {};

	UPROPERTY()
	TObjectPtr<	ABasePlayer> Player{};
public:	
	UItemUseComponent();

	UFUNCTION(BlueprintCallable, Category = "Item")
	bool UseItem(UBaseItem* Item);
	UFUNCTION(BlueprintCallable, Category = "Item")
	bool UnUseItem(UBaseItem* Item);

protected:
	virtual void BeginPlay() override;

	bool HandleDefault(UBaseItem* Item);
	bool HandleHandEquip(UBaseItem* Item);
	bool HandleHaedEquip(UBaseItem* Item);
	bool HandleBodyEquip(UBaseItem* Item);
	bool HandleConsume(UBaseItem* Item);

	bool UnHandleDefault(UBaseItem* Item);
	bool UnHandleHandEquip(UBaseItem* Item);
	bool UnHandleHeadEquip(UBaseItem* Item);
	bool UnHandleBodyEquip(UBaseItem* Item);
	bool UnHandleConsume(UBaseItem* Item);
};