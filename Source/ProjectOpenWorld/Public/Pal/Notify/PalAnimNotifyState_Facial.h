#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "PalAnimNotifyState_Facial.generated.h"

enum class EPalFacialEyeType : uint8;
enum class EPalFacialMouthType : uint8;

UCLASS()
class PROJECTOPENWORLD_API UPalAnimNotifyState_Facial : public UAnimNotifyState
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FacialEye")
	EPalFacialEyeType StartEyeType{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FacialEye")
	EPalFacialEyeType EndEyeType{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FacialMouth")
	EPalFacialMouthType StartMouthType{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FacialMouth")
	EPalFacialMouthType EndMouthType{};

public:
#if WITH_EDITOR
	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent) override;
#endif
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
