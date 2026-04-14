#pragma once

#include "CoreMinimal.h"
#include "Item/Object/ItemDataFragment.h"
#include "PlayerAnimationDataFragment.generated.h"

class UAnimSequence;

UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class PROJECTOPENWORLD_API UPlayerAnimationDataFragment : public UItemDataFragment
{
	GENERATED_BODY()
public:
};

UCLASS()
class PROJECTOPENWORLD_API UPlayerAnimationSLEDataFragment : public UPlayerAnimationDataFragment
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Animation")
	TObjectPtr<UAnimSequence> StartAnim{};
	UPROPERTY(EditAnywhere, Category = "Animation")
	TObjectPtr<UAnimSequence> LoopAnim{};
	UPROPERTY(EditAnywhere, Category = "Animation")
	TObjectPtr<UAnimSequence> EndAnim{};
public:
	FORCEINLINE UAnimSequence* GetStartAnim() const { return StartAnim.Get(); }
	FORCEINLINE UAnimSequence* GetLoopAnim() const { return LoopAnim.Get(); }
	FORCEINLINE UAnimSequence* GetEndAnim() const { return EndAnim.Get(); }
};
