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
//
//bool UAnimNotify_SphereAttack::CollisionAttackResult(USkeletalMeshComponent* MeshComp, TArray<FHitResult>& HitResult)
//{
//	return UKismetSystemLibrary::SphereTraceMultiForObjects(
//		MeshComp->GetWorld(), GetStartLocation(MeshComp), GetEndLocation(MeshComp), GetAttackCollisionShape().GetSphereRadius(),
//		{ UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel4) }, false, { MeshComp->GetOwner() }, EDrawDebugTrace::ForDuration, HitResult, true,
//		FLinearColor::Blue, FLinearColor::Green, 1.0f);
//}
