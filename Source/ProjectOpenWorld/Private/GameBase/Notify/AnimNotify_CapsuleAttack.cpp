#include "GameBase/Notify/AnimNotify_CapsuleAttack.h"

FCollisionShape UAnimNotify_CapsuleAttack::GetAttackCollisionShape() const
{
	return FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
}

FVector UAnimNotify_CapsuleAttack::GetStartLocation(USkeletalMeshComponent* MeshComp) const
{
	return MeshComp->GetSocketLocation(SocketName) + MeshComp->GetComponentRotation().Quaternion() * StartOffset;
}

FVector UAnimNotify_CapsuleAttack::GetEndLocation(USkeletalMeshComponent* MeshComp) const
{
	return  MeshComp->GetSocketLocation(SocketName) + MeshComp->GetComponentRotation().Quaternion() * EndOffset;
}

//bool UAnimNotify_CapsuleAttack::CollisionAttackResult(USkeletalMeshComponent* MeshComp, TArray<FHitResult>& HitResult)
//{
//	if (UKismetSystemLibrary::CapsuleTraceMultiForObjects(
//		MeshComp->GetWorld(), GetStartLocation(MeshComp), GetEndLocation(MeshComp),
//		GetAttackCollisionShape().GetCapsuleRadius(), GetAttackCollisionShape().GetCapsuleHalfHeight(),
//		{ UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel4) }, false, { MeshComp->GetOwner() },
//		EDrawDebugTrace::ForDuration, HitResult, true,
//		FLinearColor::Blue, FLinearColor::Green, 1.0f))
//	{
//		return true;
//	}
//	return false;
//}

