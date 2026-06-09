#include "GameBase/Component/EquipVisualComponent.h"
#include "GameFramework/Character.h"
#include "Item/DataAsset/ItemDataAsset.h"

UEquipVisualComponent::UEquipVisualComponent() : Super()
{
	SetCollisionProfileName(TEXT("NoCollision"));
}

void UEquipVisualComponent::BeginPlay()
{
	Super::BeginPlay();
	UHandEquipItemFragment::UnEquipItem(this);
}

#if WITH_EDITOR
void UEquipVisualComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	FName AssetName = PropertyChangedEvent.GetPropertyName();
	if (AssetName == GET_MEMBER_NAME_CHECKED(UEquipVisualComponent, EquipDataAsset))
	{
		if (EquipDataAsset)
		{
			if (ACharacter* Player = Cast<ACharacter>(GetOwner()))
			{
				TObjectPtr < UItemDataFragment> Fragments = EquipDataAsset->GetItemDataFragmentOfClass(UHandEquipItemFragment::StaticClass());
				if (UHandEquipItemFragment* HandEquipFragment = Cast<UHandEquipItemFragment>(Fragments))
				{
					FItemEquipContext Context{};
					Context.User = Player;
					Context.Target = this;
					Context.Item = nullptr;
					HandEquipFragment->Equip(Context);
				}
			}
		}
		else
		{
			UHandEquipItemFragment::UnEquipItem(this);
		}
	}
}
#endif