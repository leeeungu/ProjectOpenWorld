#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_CreatureAttack.generated.h"

UCLASS(Abstract)
class PROJECTOPENWORLD_API UAnimNotify_CreatureAttack : public UAnimNotify
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (AnimNotifyBoneName = "true"))
	FName SocketName{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FVector SocketOffset{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	float AttackRadius = 100.0f;

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
