#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemDataFragment.generated.h"


UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class PROJECTOPENWORLD_API UItemDataFragment : public UObject
{
	GENERATED_BODY()
// 각 클래스가 하나만 들어 갈 수 있도록 하려고 했음
// UHandEquipItemFragment가 그 대상이라 생각했음 근데 그러면  양손 무기는?(하나의 mesh가 아닌데?) => 그냥 하지말자
//protected:
//	bool bIsDuplicatable{ false };
};

class USkeletalMesh;

UCLASS()
class UHandEquipItemFragment : public UItemDataFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "HandEquip")
	TObjectPtr<USkeletalMesh> HandEquipMesh{};

	UPROPERTY(EditAnywhere, Category = "HandEquip")
	FName HandEquipSocket{};

	UPROPERTY(EditAnywhere, Category = "HandEquip")
	FTransform HandEquipRelativeTransform{};

public:
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