#include "Player/Component/PlayerItemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Item/Object/BaseItemObject.h"

UPlayerItemComponent::UPlayerItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	ItemActor.Empty();
}

void UPlayerItemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerItemComponent::RegisterItemActor(TSubclassOf<UBaseItemObject> InItemObject)
{
	if (InItemObject)
	{
		bool bAlreadyIn = false;
		ItemActor.FindOrAdd(InItemObject, &bAlreadyIn);
		if (!bAlreadyIn)
		{
			InItemObject.GetDefaultObject()->RegisterItemObject(GetOwner());
		}
	}
}

void UPlayerItemComponent::UnRegisterItemActor(TSubclassOf<UBaseItemObject> InItemObject)
{
	if (InItemObject)
	{
		if (ItemActor.Remove(InItemObject) > 0)
		{
			InItemObject.GetDefaultObject()->UnregisterItemObject(GetOwner());
		}
	}
}