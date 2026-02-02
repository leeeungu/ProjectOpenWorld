#pragma once

#include "CoreMinimal.h"
#include "GameBase/Notify/AnimNotify_Attack.h"
#include "AnimNotify_CapsuleAttack.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UAnimNotify_CapsuleAttack : public UAnimNotify_Attack
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (AnimNotifyBoneName = "true"))
	FName SocketName{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FVector StartOffset{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FVector EndOffset{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FRotator CapsuleRotationOffset{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ClampMin = 0.0f))
	float CapsuleRadius = 60.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ClampMin = 0.0f))
	float CapsuleHalfHeight = 90.0f;

	virtual FCollisionShape GetAttackCollisionShape() const override;
	virtual FVector GetStartLocation(USkeletalMeshComponent* MeshComp) const override;
	virtual FVector GetEndLocation(USkeletalMeshComponent* MeshComp) const override;
	//virtual bool CollisionAttackResult(USkeletalMeshComponent* MeshComp, TArray<FHitResult>& HitResult) override;
};
