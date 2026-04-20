#pragma once

#include "CoreMinimal.h"
#include "Item/Object/ItemDataFragment.h"
#include "Item/DataTable/ItemSlotType.h"
#include "ItemDataSlotFragment.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UItemDataSlotFragment : public UItemDataFragment
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SlotData")
	EItemSlotType SlotType{ EItemSlotType::None };

public:
	EItemSlotType GetSlotType() const { return SlotType; }
};
