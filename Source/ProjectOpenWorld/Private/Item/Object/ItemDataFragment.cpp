#include "Item/Object/ItemDataFragment.h"
#include "Item/AssetUserData/WeaponeAssetUserData.h"
#include "GameFramework/Character.h"
#include "Engine/SkeletalMesh.h"


bool UHandEquipItemFragment::Equip(const FItemEquipContext& Context) const
{
	HandEquipMesh.LoadSynchronous();
	return EquipItem(Context.Target, HandEquipMesh.Get(), HandEquipSocket, HandEquipRelativeTransform);
}

bool UHandEquipItemFragment::Unequip(const FItemEquipContext& Context) const
{
	return UnEquipItem(Context.Target);
}

bool UHandEquipItemFragment::EquipItem(USkeletalMeshComponent* Target, USkeletalMesh* TargetMesh, FName SockeName, FTransform Relative)
{
	if (!Target || !TargetMesh || !Target->GetOwner())
		return false;
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(Target->GetOwner()))
	{
		Target->SetSkeletalMesh(TargetMesh);
		Target->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SockeName);
		Target->SetRelativeTransform(Relative);
		return true;
	}
	return false;
}

bool UHandEquipItemFragment::UnEquipItem(USkeletalMeshComponent* Target)
{
	if (!Target)
		return false;
	Target->SetSkeletalMesh(nullptr);
	Target->SetRelativeTransform(FTransform::Identity);
	return true;
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
