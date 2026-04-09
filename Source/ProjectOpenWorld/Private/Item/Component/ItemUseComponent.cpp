#include "Item/Component/ItemUseComponent.h"
#include "Item/Component/EquipmentComponent.h"
#include "Item/Object/BaseItem.h"
#include "Player/Character/BasePlayer.h"

UItemUseComponent::UItemUseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	UseHandler[static_cast<uint8>(EItemUseType::None)] = &UItemUseComponent::HandleDefault;
	UseHandler[static_cast<uint8>(EItemUseType::HandEquip)] = &UItemUseComponent::HandleHandEquip;
	UseHandler[static_cast<uint8>(EItemUseType::ArmorEquip)] = &UItemUseComponent::HandleArmorEquip;
	UseHandler[static_cast<uint8>(EItemUseType::Consume)] = &UItemUseComponent::HandleConsume;
	UseHandler[static_cast<uint8>(EItemUseType::Throw)] = &UItemUseComponent::HandleThrow;
}

void UItemUseComponent::BeginPlay()
{
	Super::BeginPlay();
	ABasePlayer* Player = Cast<ABasePlayer>(GetOwner());
	EquipmentComponent = Player ? Player->GetPlayerEquipComponent() : nullptr;
}

bool UItemUseComponent::UseItem(UBaseItem* Item)
{
	if (Item)
	{
		EItemUseType UseType = Item->GetUseType();
		uint8 UseTypeValue = static_cast<uint8>(UseType);
		if (UseHandler[UseTypeValue])
			return (this->*UseHandler[UseTypeValue])(Item);
	}
	return false;
}

bool UItemUseComponent::HandleDefault(UBaseItem* Item)
{
	return false;
}

bool UItemUseComponent::HandleHandEquip(UBaseItem* Item)
{
	if (EquipmentComponent)
	{
		return EquipmentComponent->EquipItem(Item);
	}
	return false;
}

bool UItemUseComponent::HandleArmorEquip(UBaseItem* Item)
{
	return false;
}

bool UItemUseComponent::HandleConsume(UBaseItem* Item)
{
	return false;
}

bool UItemUseComponent::HandleThrow(UBaseItem* Item)
{
	return false;
}

