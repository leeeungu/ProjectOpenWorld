#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_PatternEvent.generated.h"

UENUM(BlueprintType)
enum class EPalPatternEventType: uint8
{
	PatternStart UMETA(DisplayName = "PatternStart"),
	PatternEnd UMETA(DisplayName = "PatternEnd"),
};

UCLASS(Abstract)
class PROJECTOPENWORLD_API UAnimNotify_PatternEvent : public UAnimNotify
{
	GENERATED_BODY()


protected:
	UPROPERTY(EditAnywhere, Category = "PatternEvent")
	 EPalPatternEventType PatternState{};
	uint8 Index{};
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
