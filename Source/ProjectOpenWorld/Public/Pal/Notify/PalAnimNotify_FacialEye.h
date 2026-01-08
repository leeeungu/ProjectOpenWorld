#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "PalAnimNotify_FacialEye.generated.h"

enum class EPalFacialEyeType : uint8;

UCLASS()
class PROJECTOPENWORLD_API UPalAnimNotify_FacialEye : public UAnimNotify
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Facial")
	 EPalFacialEyeType EyeType;
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
