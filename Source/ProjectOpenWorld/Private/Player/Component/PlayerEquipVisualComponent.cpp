#include "Player/Component/PlayerEquipVisualComponent.h"
#include "Item/DataAsset/ItemDataAsset.h"
#include "GameFramework/Character.h"

UPlayerEquipVisualComponent::UPlayerEquipVisualComponent() : Super()
{
	SetCollisionProfileName(TEXT("NoCollision"));
}

void UPlayerEquipVisualComponent::BeginPlay()
{
	Super::BeginPlay();
	SetSkeletalMesh(nullptr);
}


#if WITH_EDITOR
void UPlayerEquipVisualComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	FName AssetName = PropertyChangedEvent.GetPropertyName();
	if (AssetName == GET_MEMBER_NAME_CHECKED(UPlayerEquipVisualComponent, EquipDataAsset))
	{
		ACharacter* Player = Cast<ACharacter>(GetOwner());
		if (Player)
		{
			if (EquipDataAsset)
			{
				TObjectPtr < UItemDataFragment> Fragments = EquipDataAsset->GetItemDataFragmentOfClass(UHandEquipItemFragment::StaticClass());
				if (UHandEquipItemFragment* HandEquipFragment = Cast<UHandEquipItemFragment>(Fragments))
				{
					SetSkeletalMesh(HandEquipFragment->HandEquipMesh);
					AttachToComponent(
						Player->GetMesh(),
						FAttachmentTransformRules::KeepRelativeTransform,
						HandEquipFragment->HandEquipSocket);
					SetRelativeTransform(HandEquipFragment->HandEquipRelativeTransform);
				}
			}
			else
			{
				SetSkeletalMesh(nullptr);
				SetRelativeTransform(FTransform::Identity);
			}
		}
	}
}
#endif