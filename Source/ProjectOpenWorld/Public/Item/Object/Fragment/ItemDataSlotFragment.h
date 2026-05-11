#pragma once

#include "CoreMinimal.h"
#include "Item/Object/ItemDataFragment.h"
#include "Item/DataTable/ItemSlotType.h"
#include "ItemDataSlotFragment.generated.h"

enum class EWeapone : uint8;
enum class EPlayerState : uint8;

UCLASS()
class PROJECTOPENWORLD_API UItemDataSlotFragment : public UItemDataFragment
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SlotData")
	EItemSlotType SlotType{ EItemSlotType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapone")
	EWeapone WeaponeData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapone")
	EPlayerState EquipPlayerState{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapone")
	EPlayerState UnEquipPlayerState{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapone")
	FName WeaponeID{};

public:
	EItemSlotType GetSlotType() const { return SlotType; }
	EWeapone GetWeaponeData() const { return WeaponeData; }
	EPlayerState GetEquipPlayerState() const { return EquipPlayerState; }
	EPlayerState GetUnEquipPlayerState() const { return UnEquipPlayerState; }
	FName GetWeaponeID() const { return WeaponeID; }
};
