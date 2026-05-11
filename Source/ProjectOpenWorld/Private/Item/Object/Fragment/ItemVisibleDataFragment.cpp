#include "Item/Object/Fragment/ItemVisibleDataFragment.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"

UItemVisibleDataFragment::UItemVisibleDataFragment() : 
	UItemDataFragment{}, VisibleMeshType(EItemVisibleMeshType::None), MeshAsset(nullptr)
{
	ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshAsset(TEXT("/Game/Pal/Model/Prop/Resource/CoinGold/Mesh/SM_CoinGold.SM_CoinGold"));
	if(StaticMeshAsset.Succeeded())
	{
		MeshAsset = StaticMeshAsset.Object;
		VisibleMeshType = EItemVisibleMeshType::StaticMesh;
	}
}

#if WITH_EDITOR
void UItemVisibleDataFragment::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if(PropertyName == GET_MEMBER_NAME_CHECKED(UItemVisibleDataFragment, MeshAsset))
	{
		if (MeshAsset)
		{
			if (MeshAsset->IsA<UStaticMesh>())
			{
				VisibleMeshType = EItemVisibleMeshType::StaticMesh;
			}
			else if (MeshAsset->IsA<USkeletalMesh>())
			{
				VisibleMeshType = EItemVisibleMeshType::SkeletalMesh;
			}
			else
			{
				VisibleMeshType = EItemVisibleMeshType::None;
				MeshAsset = nullptr;
			}
		}
		else
		{
			VisibleMeshType = EItemVisibleMeshType::None;
			MeshAsset = nullptr;
		}
	}
}
#endif

UStaticMesh* UItemVisibleDataFragment::GetStaticMesh() const
{
	if(VisibleMeshType != EItemVisibleMeshType::StaticMesh)
	{
		return nullptr;
	}
	return Cast<UStaticMesh>(MeshAsset.LoadSynchronous());
}
USkeletalMesh* UItemVisibleDataFragment::GetSkeletalMesh() const
{
	if(VisibleMeshType != EItemVisibleMeshType::SkeletalMesh)
	{
		return nullptr;
	}
	return Cast<USkeletalMesh>(MeshAsset.LoadSynchronous());
}
