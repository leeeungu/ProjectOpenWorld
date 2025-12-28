#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Engine/EngineTypes.h"
#include "AnimNotifyState_SetMovementMode.generated.h"


UCLASS()
class PROJECTOPENWORLD_API UAnimNotifyState_SetMovementMode : public UAnimNotifyState
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	TEnumAsByte<EMovementMode> NewMovementMode = EMovementMode::MOVE_Flying;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	uint8 NewCustomMode{};

public:
#if WITH_EDITOR
	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent) override;
#endif
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
