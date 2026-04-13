#pragma once

#include "CoreMinimal.h"
#include "Item/Object/ItemDataFragment.h"
#include "ItemVisibleDataFragment.generated.h"

class UStreamableRenderAsset;
class UStaticMesh;
class USkeletalMesh;

UENUM(BlueprintType)
enum class EItemVisibleMeshType : uint8
{
	None,
	StaticMesh,
	SkeletalMesh,
	MeshTypeMax UMETA(Hidden)
};

UCLASS()
class PROJECTOPENWORLD_API UItemVisibleDataFragment : public UItemDataFragment
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Visible")
	EItemVisibleMeshType VisibleMeshType{ EItemVisibleMeshType::None };

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Visible", meta = (AllowedClasses = "/Script/Engine.StaticMesh,/Script/Engine.SkeletalMesh"))
	TObjectPtr<	UStreamableRenderAsset> MeshAsset{};
public:
	UItemVisibleDataFragment();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	FORCEINLINE EItemVisibleMeshType GetVisibleMeshType() const { return VisibleMeshType; }
	FORCEINLINE UStaticMesh* GetStaticMesh() const;
	FORCEINLINE USkeletalMesh* GetSkeletalMesh() const;
};
