#include "GameBase/Notify/AnimNotify_BoxAttack.h"
#include "kismet/KismetSystemLibrary.h"

FCollisionShape UAnimNotify_BoxAttack::GetAttackCollisionShape() const
{
	return FCollisionShape::MakeBox(HalfSize);
}

FVector UAnimNotify_BoxAttack::GetStartLocation(USkeletalMeshComponent* MeshComp) const
{
	return MeshComp->GetSocketLocation(SocketName) + BoxRotation.Quaternion() * StartOffset;
}

FVector UAnimNotify_BoxAttack::GetEndLocation(USkeletalMeshComponent* MeshComp) const
{
	return  MeshComp->GetSocketLocation(SocketName) + BoxRotation.Quaternion() * EndOffset;
}

bool UAnimNotify_BoxAttack::CollisionAttackResult(USkeletalMeshComponent* MeshComp, TArray<FHitResult>& HitResult)
{
	return UKismetSystemLibrary::BoxTraceMultiForObjects(
		MeshComp->GetWorld(), GetStartLocation(MeshComp), GetEndLocation(MeshComp),
		GetAttackCollisionShape().GetBox() * 0.5f, BoxRotation, {UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel4)}, false, {MeshComp->GetOwner()},
		EDrawDebugTrace::ForDuration, HitResult, true,
		FLinearColor::Blue, FLinearColor::Green, 1.0f);
}
