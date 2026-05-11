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
	TSoftObjectPtr<UAnimSequence> StartAnim{};
	UPROPERTY(EditAnywhere, Category = "Animation")
	TSoftObjectPtr<UAnimSequence> LoopAnim{};
	UPROPERTY(EditAnywhere, Category = "Animation")
	TSoftObjectPtr<UAnimSequence> EndAnim{};
public:
	UAnimSequence* GetStartAnim() const;
	UAnimSequence* GetLoopAnim() const;
	UAnimSequence* GetEndAnim() const;
};
