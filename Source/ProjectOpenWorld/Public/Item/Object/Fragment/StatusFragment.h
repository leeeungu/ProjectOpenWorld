#pragma once

#include "CoreMinimal.h"
#include "Item/Object/ItemDataFragment.h"
#include "StatusFragment.generated.h"

enum class EStatusType :uint8;

// 공유 에셋에 들어가는 "정의" — Min==Max면 고정값, 다르면 [Min,Max] 랜덤
USTRUCT(BlueprintType)
struct FStatModifierRange
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "StatMod")
	EStatusType StatType{};

	UPROPERTY(EditAnywhere, Category = "StatMod")
	float MinValue = 0.f;

	UPROPERTY(EditAnywhere, Category = "StatMod")
	float MaxValue = 0.f;
};

// per-instance에 저장되는 "굴린 결과" — UBaseItem이 들고 세이브됨
USTRUCT(BlueprintType)
struct FRolledStatModifier
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, SaveGame, Category = "StatMod")
	EStatusType StatType{};

	UPROPERTY(VisibleAnywhere, SaveGame, Category = "StatMod")
	float Value = 0.f;
};

UCLASS()
class PROJECTOPENWORLD_API UStatusFragment : public UItemDataFragment, public IUseItemInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "StatMod")
	TMap<EStatusType, float> Modifiers{};
public:
	virtual void OnUse(const FItemUseContext& Ctx, FItemUseResult& Out) const override;
	virtual void OnUnUse(const FItemUseContext& Ctx, FItemUseResult& Out) const override;
};
