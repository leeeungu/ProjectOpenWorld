#include "Item/Component/ItemUseComponent.h"
#include "Player/Character/BasePlayer.h"
#include "Item/Component/EquipmentComponent.h"
#include "Item/Object/BaseItem.h"
#include "Item/DataAsset/ItemDataAsset.h"

UItemUseComponent::UItemUseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	UseHandler[static_cast<uint8>(EItemUseType::None)] = &UItemUseComponent::HandleDefault;
	UseHandler[static_cast<uint8>(EItemUseType::HandEquip)] = &UItemUseComponent::HandleHandEquip;
	UseHandler[static_cast<uint8>(EItemUseType::HeadEquip)] = &UItemUseComponent::HandleHaedEquip;
	UseHandler[static_cast<uint8>(EItemUseType::Consume)] = &UItemUseComponent::HandleConsume;

	UnUseHandler[static_cast<uint8>(EItemUseType::None)] = &UItemUseComponent::UnHandleDefault;
	UnUseHandler[static_cast<uint8>(EItemUseType::HandEquip)] = &UItemUseComponent::UnHandleHandEquip;
	UnUseHandler[static_cast<uint8>(EItemUseType::HeadEquip)] = &UItemUseComponent::UnHandleHeadEquip;
	UnUseHandler[static_cast<uint8>(EItemUseType::Consume)] = &UItemUseComponent::UnHandleConsume;
}

void UItemUseComponent::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<ABasePlayer>(GetOwner());
}

bool UItemUseComponent::UseItem(UBaseItem* Item)
{
	if (Item && Player)
	{
		EItemUseType UseType = Item->GetUseType();
		uint8 UseTypeValue = static_cast<uint8>(UseType);
		if (UseHandler[UseTypeValue])
			return (this->*UseHandler[UseTypeValue])(Item);
	}
	return false;
}

bool UItemUseComponent::UnUseItem(UBaseItem* Item)
{
	if (Item && Player)
	{
		EItemUseType UseType = Item->GetUseType();
		uint8 UseTypeValue = static_cast<uint8>(UseType);
		if (UnUseHandler[UseTypeValue])
			return (this->*UnUseHandler[UseTypeValue])(Item);
	}
	return false;
}

bool UItemUseComponent::HandleDefault(UBaseItem* Item)
{
	return false;
}

bool UItemUseComponent::HandleHandEquip(UBaseItem* Item)
{
	if (UEquipmentComponent* EquipmentComponent = Player->GetPlayerEquipComponent())
	{
		return EquipmentComponent->EquipItem(Item);
	}
	return false;
}

bool UItemUseComponent::HandleHaedEquip(UBaseItem* Item)
{
	return false;
}
bool UItemUseComponent::HandleBodyEquip(UBaseItem* Item)
{
	return false;
}
bool UItemUseComponent::HandleConsume(UBaseItem* Item)
{
	return false;
}

bool UItemUseComponent::UnHandleDefault(UBaseItem* Item)
{
	return false;
}

bool UItemUseComponent::UnHandleHandEquip(UBaseItem* Item)
{
	if (UEquipmentComponent* EquipmentComponent = Player->GetPlayerEquipComponent())
	{
		return EquipmentComponent->UnequipItem(Item);
	}
	return false;
}

bool UItemUseComponent::UnHandleHeadEquip(UBaseItem* Item)
{
	return false;
}

bool UItemUseComponent::UnHandleBodyEquip(UBaseItem* Item)
{
	return false;
}

bool UItemUseComponent::UnHandleConsume(UBaseItem* Item)
{
	return false;
}

