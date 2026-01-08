#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "PalAnimNotify_FacialMouth.generated.h"

enum class EPalFacialMouthType : uint8;

UCLASS()
class PROJECTOPENWORLD_API UPalAnimNotify_FacialMouth : public UAnimNotify
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Facial")
	EPalFacialMouthType MouthType;

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
