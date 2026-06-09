#pragma once

#include "CoreMinimal.h"
#include "Item/Object/ItemDataFragment.h"
#include "PlayerAnimationDataFragment.generated.h"

class UAnimSequence;

UCLASS(BlueprintType)
class PROJECTOPENWORLD_API UPlayerAnimationSLEDataFragment : public UItemDataFragment, public IEquipItemInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Animation")
	TSoftObjectPtr<UAnimSequence> StartAnim{};
	UPROPERTY(EditAnywhere, Category = "Animation")
	TSoftObjectPtr<UAnimSequence> LoopAnim{};
	UPROPERTY(EditAnywhere, Category = "Animation")
	TSoftObjectPtr<UAnimSequence> EndAnim{};
public:
	UAnimSequence* GetStartAnim() const;
	UAnimSequence* GetLoopAnim() const;
	UAnimSequence* GetEndAnim() const;


	virtual bool Equip(const FItemEquipContext& Context) const override;
	virtual bool Unequip(const FItemEquipContext& Context) const override;
};
