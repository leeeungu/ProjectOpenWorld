#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_Attack.generated.h"

class UAttackObject;

UCLASS(Abstract)
class PROJECTOPENWORLD_API UAnimNotify_Attack : public UAnimNotify
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", Instanced)
	TArray< TObjectPtr<UAttackObject>> AttackEventObject{};
public:
#if WITH_EDITOR
	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent) override;
#endif
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

protected:
	virtual FCollisionShape GetAttackCollisionShape() const;
	virtual FVector GetStartLocation(USkeletalMeshComponent* MeshComp) const;
	virtual FVector GetEndLocation(USkeletalMeshComponent* MeshComp) const;
	virtual bool CollisionAttackResult(USkeletalMeshComponent* MeshComp, TArray<FHitResult>& HitResult);
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	void AttackEventObjectDebug(USkeletalMeshComponent* MeshComp);
};
