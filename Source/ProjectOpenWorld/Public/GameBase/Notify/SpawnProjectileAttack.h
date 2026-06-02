#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Pal/Data/PalDamageType.h"
#include "SpawnProjectileAttack.generated.h"

class AProjectileAttack;

UCLASS()
class PROJECTOPENWORLD_API USpawnProjectileAttack : public UAnimNotify
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "ProjectileAttack ")
	TSubclassOf<AProjectileAttack > AttackProjectile{};
	UPROPERTY(EditAnywhere, Category = "ProjectileAttack ", meta = (AnimNotifyBoneName = "true"))
	FName SocketName{};
	UPROPERTY(EditAnywhere, Category = "ProjectileAttack ")
	FVector LocationOffset{};
	UPROPERTY(EditAnywhere, Category = "ProjectileAttack ")
	FRotator RotationOffset{};
	UPROPERTY(EditAnywhere, Category = "ProjectileAttack ")
	FVector Scale = FVector::OneVector;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	FProjectileLaunchParams LaunchParams{};

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
