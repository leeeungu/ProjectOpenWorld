#include "Creature/Notify/AnimNotify_CreatureAttack.h"
#include "Creature/Interface/CreatureAttackInterface.h"
#include "GameFramework/Pawn.h"
#include "Engine/DamageEvents.h"
#include "DrawDebugHelpers.h"

void UAnimNotify_CreatureAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	UAnimNotify::Notify(MeshComp, Animation, EventReference);
	TScriptInterface<ICreatureAttackInterface> AttackInterface = TScriptInterface<ICreatureAttackInterface>(MeshComp->GetOwner());
	FVector NewLocation = MeshComp->GetSocketLocation(SocketName) + MeshComp->GetComponentRotation().Quaternion() * SocketOffset;
	if (AttackInterface && MeshComp->GetOwner())
	{
		TArray<FHitResult> arResult{};
		FCollisionShape Collision = FCollisionShape::MakeSphere(AttackRadius);
		FCollisionQueryParams Param{};
		FCollisionResponseParams ResponseParam{};
		Param.AddIgnoredActor(MeshComp->GetOwner());
		if(MeshComp->GetWorld()->SweepMultiByChannel(arResult, NewLocation, NewLocation, FQuat::Identity, ECollisionChannel::ECC_Pawn,
			Collision, Param))
		{
			for (const FHitResult& Hit : arResult)
			{
				Hit.GetActor()->TakeDamage(ICreatureAttackInterface::Execute_GetAttackDamage(MeshComp->GetOwner()), FDamageEvent{},
					MeshComp->GetOwner()->GetInstigatorController(), MeshComp->GetOwner());
			}
		}
	}
	else
	{
#if WITH_EDITOR	
		DrawDebugSphere(MeshComp->GetWorld(), NewLocation, AttackRadius, 20, FColor::Red, false, 0.5f, 0 , 0.5f);
#endif
	}
}
