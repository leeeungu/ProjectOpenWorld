#include "GameBase/Notify/AnimNotify_SphereAttack.h"

FCollisionShape UAnimNotify_SphereAttack::GetAttackCollisionShape() const
{
	return FCollisionShape::MakeSphere(AttackRadius);
}

FVector UAnimNotify_SphereAttack::GetStartLocation(USkeletalMeshComponent* MeshComp) const
{
	return MeshComp->GetSocketLocation(SocketName) + MeshComp->GetComponentRotation().Quaternion() * StartOffset;
}

FVector UAnimNotify_SphereAttack::GetEndLocation(USkeletalMeshComponent* MeshComp) const
{
	return  MeshComp->GetSocketLocation(SocketName) + MeshComp->GetComponentRotation().Quaternion() * EndOffset;
}
