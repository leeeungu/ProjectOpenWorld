#include "Item/Object/ItemDataFragment.h"
#include "Item/AssetUserData/WeaponeAssetUserData.h"
#include "Engine/SkeletalMesh.h"

USkeletalMesh* UHandEquipItemFragment::GetHandEquipMesh() const
{
	return HandEquipMesh.LoadSynchronous();
}

#if WITH_EDITOR
void UHandEquipItemFragment::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UHandEquipItemFragment, HandEquipMesh))
	{
		if (HandEquipMesh.IsValid())
		{
			USkeletalMesh* Mesh = HandEquipMesh.Get();
			UWeaponeAssetUserData* Data = Cast< UWeaponeAssetUserData>(Mesh->GetAssetUserDataOfClass(UWeaponeAssetUserData::StaticClass()));
			if (Data)
			{
				HandEquipSocket = Data->GetSocketName();
				HandEquipRelativeTransform.SetLocation(FVector::ZeroVector);
				HandEquipRelativeTransform.SetRotation(FQuat::Identity);
				HandEquipRelativeTransform.SetScale3D(FVector::OneVector);
				return;
			}
		}
		HandEquipSocket = NAME_None;
		HandEquipRelativeTransform.SetLocation(FVector::ZeroVector);
		HandEquipRelativeTransform.SetRotation(FQuat::Identity);
		HandEquipRelativeTransform.SetScale3D(FVector::OneVector);
	}
}
#endif
