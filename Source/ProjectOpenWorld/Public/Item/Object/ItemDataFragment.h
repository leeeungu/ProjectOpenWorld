#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemDataFragment.generated.h"


UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class PROJECTOPENWORLD_API UItemDataFragment : public UObject
{
	GENERATED_BODY()
// DatatAsset에 instance 배열안에는 클래스 당  하나만 들어 갈 수 있도록 하려고 했음
// UHandEquipItemFragment가 그 대상이라 생각했음 => 같은 손에 Mesh가 여러개 걸리면 곤란하니깐
// 근데 그러면 양손 무기는?(하나의 mesh가 아닌데?) => 해결이 어려워 지니 폐기
//protected:
//	bool bIsDuplicatable{ false };
};

class USkeletalMesh;

UCLASS()
class UHandEquipItemFragment : public UItemDataFragment
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "HandEquip")
	TSoftObjectPtr<USkeletalMesh> HandEquipMesh{};

	UPROPERTY(EditAnywhere, Category = "HandEquip")
	FName HandEquipSocket{};

	UPROPERTY(EditAnywhere, Category = "HandEquip")
	FTransform HandEquipRelativeTransform{};

public:
	USkeletalMesh* GetHandEquipMesh() const;
	FName GetHandEquipSocket() const { return HandEquipSocket; }
	FTransform GetHandEquipRelativeTransform() const { return HandEquipRelativeTransform; }
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};


UCLASS()
class UComsumItemFragment : public UItemDataFragment
{
	GENERATED_BODY()
public:
	//UPROPERTY(EditAnywhere, Category = "Consume")
	//FName ConsumeEffectID{};

	UPROPERTY(EditAnywhere, Category = "Consume", meta = (ClampMin = 0.0f))
	float Amount{ 0.f };
};