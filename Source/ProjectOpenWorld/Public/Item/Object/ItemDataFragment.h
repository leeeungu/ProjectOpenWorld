#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemDataFragment.generated.h"

class AActor;
class UBaseItem;

USTRUCT()
struct FItemUseContext
{
	GENERATED_USTRUCT_BODY()
	TObjectPtr<AActor> User{};
	TObjectPtr<UBaseItem> ItemInstance{};
};


USTRUCT()
struct FItemUseResult
{
	GENERATED_USTRUCT_BODY()
	bool  bSuccess = false;
	int32 ConsumeCount = 0;
};

UINTERFACE(MinimalAPI)
class UUseItemInterface : public UInterface { GENERATED_BODY() };
class IUseItemInterface
{
	GENERATED_BODY()
public:
	// 공유 에셋이라 const + Context 그대로 유지
	virtual void OnUse(const FItemUseContext& Ctx, FItemUseResult& Out) const {};
	virtual void OnUnUse(const FItemUseContext& Ctx, FItemUseResult& Out) const {};
};

//UINTERFACE(MinimalAPI)
//class UItemAcquireReactor : public UInterface { GENERATED_BODY() };
//class IItemAcquireReactor
//{
//	GENERATED_BODY()
//public:
//	virtual void OnAcquire(const FItemUseContext& Ctx) const = 0;
//};

class USkeletalMeshComponent;

USTRUCT()
struct FItemEquipContext
{
	GENERATED_USTRUCT_BODY()
	TObjectPtr<AActor> User{};
	TObjectPtr<USkeletalMeshComponent> Target{};
	const UBaseItem* Item{};
};

UINTERFACE(MinimalAPI)
class UEquipItemInterface : public UInterface { GENERATED_BODY() };
class IEquipItemInterface
{
	GENERATED_BODY()
public:
	virtual bool Equip(const FItemEquipContext& Context) const { return false; }
	virtual bool Unequip(const FItemEquipContext& Context) const { return false; }
};


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
class UHandEquipItemFragment : public UItemDataFragment, public IEquipItemInterface
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
	virtual bool Equip(const FItemEquipContext& Context) const override;
	virtual bool Unequip(const FItemEquipContext& Context) const override;

	static bool EquipItem(USkeletalMeshComponent* Target, class USkeletalMesh* TargetMesh, FName SockeName, FTransform Relative);
	static bool UnEquipItem(USkeletalMeshComponent* Target);
	//USkeletalMesh* GetHandEquipMesh() const;
	//FName GetHandEquipSocket() const { return HandEquipSocket; }
	//FTransform GetHandEquipRelativeTransform() const { return HandEquipRelativeTransform; }
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};


UCLASS()
class UComsumItemFragment : public UItemDataFragment, public IUseItemInterface
{
	GENERATED_BODY()
public:
	//UPROPERTY(EditAnywhere, Category = "Consume")
	//FName ConsumeEffectID{};

	UPROPERTY(EditAnywhere, Category = "Consume", meta = (ClampMin = 0.0f))
	float Amount{ 0.f };
};