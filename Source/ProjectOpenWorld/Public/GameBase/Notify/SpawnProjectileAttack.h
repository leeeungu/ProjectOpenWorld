#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SpawnProjectileAttack.generated.h"

class AProjectileAttack;

UCLASS()
class PROJECTOPENWORLD_API USpawnProjectileAttack : public UAnimNotify
{
	GENERATED_BODY()


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileAttack ")
	TSubclassOf<AProjectileAttack > AttackProjectile{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileAttack ", meta = (AnimNotifyBoneName = "true"))
	FName SocketName{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileAttack ")
	FVector LocationOffset{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileAttack ")
	FRotator RotationOffset{};
	UPROPERTY(EditAnywhere, Category = "ProjectileAttack ")
	FVector Scale = FVector::OneVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileAttack ")
	FVector ProjectileDirection = FVector::ForwardVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileAttack ")
	float ProjectileSpeed = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileAttack ")
	float ProjectileLifeTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileAttack ")
	float ProjectileDistanceTraveled = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileAttack ")
	bool bUseMeshRotation = true;


	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
