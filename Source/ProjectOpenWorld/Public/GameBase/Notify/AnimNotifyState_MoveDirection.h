#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_MoveDirection.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UAnimNotifyState_MoveDirection : public UAnimNotifyState
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveDirection")
	FVector MoveDirection{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveDirection")
	float MoveSpeed{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveDirection")
	bool bIsWorldSpace{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveDirection")
	bool bIsIgnoreZ{};

#if WITH_EDITOR
	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent) override;
#endif

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	
};
