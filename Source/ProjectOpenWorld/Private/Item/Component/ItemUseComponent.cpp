#include "Item/Component/ItemUseComponent.h"
#include "Player/Character/BasePlayer.h"
#include "Item/Component/EquipmentComponent.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Item/Object/BaseItem.h"
#include "Item/DataAsset/ItemDataAsset.h"
#include "Item/System/ItemDataSubsystem.h"

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
		UItemDataAsset* Asset = Item->GetPalItemDataAssetByName();
		if (Asset)
		{
			FItemUseContext Context{};
			Context.User = Player;
			Context.ItemInstance = Item;
			FItemUseResult Result{};
			const TArray<TObjectPtr<UItemDataFragment>>* Array = Asset->GetItemDataFragmentArray();
			for (auto& Fragment : *Array)
			{
				if (IUseItemInterface* UseInterface = Cast< IUseItemInterface>(Fragment))
				{
					UseInterface->OnUse(Context, Result);
				}
			}
			EItemUseType UseType = Item->GetUseType();
			uint8 UseTypeValue = static_cast<uint8>(UseType);
			if (UseHandler[UseTypeValue])
				return (this->*UseHandler[UseTypeValue])(Item);
		}
		else if (Player->GetInventoryComponent())
		{
			Player->GetInventoryComponent()->AddItem(TEXT("Money"), Item->GetItemCount());
			return true;
		}
	}
	return false;
}

bool UItemUseComponent::UnUseItem(UBaseItem* Item)
{
	if (Item && Player)
	{
		if (UItemDataAsset* Asset = Item->GetPalItemDataAssetByName())
		{
			FItemUseContext Context{};
			Context.User = Player;
			Context.ItemInstance = Item;
			FItemUseResult Result{};
			const TArray<TObjectPtr<UItemDataFragment>>* Array = Asset->GetItemDataFragmentArray();
			for (auto& Fragment : *Array)
			{
				if (IUseItemInterface* UseInterface = Cast< IUseItemInterface>(Fragment))
				{
					UseInterface->OnUnUse(Context, Result);
				}
			}
		}
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
		return EquipmentComponent->RegisterItem(Item);
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
	//UEquipmentComponent
	if (UEquipmentComponent* EquipmentComponent = Player->GetPlayerEquipComponent())
	{
		return EquipmentComponent->UnRegisterItem(Item);
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

