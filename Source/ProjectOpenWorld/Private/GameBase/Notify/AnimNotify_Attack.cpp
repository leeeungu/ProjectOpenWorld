#include "GameBase/Notify/AnimNotify_Attack.h"
#include "GameBase/Interface/AttackInterface.h"
#include "Kismet/GameplayStatics.h"
#include "GenericTeamAgentInterface.h"
#include "GameBase/Object/AttackObject.h"

void UAnimNotify_Attack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	FVector NewLocation = MeshComp->GetSocketLocation(SocketName) + MeshComp->GetComponentRotation().Quaternion() * SocketOffset;
	UWorld* pWorld = MeshComp->GetWorld();
	if (MeshComp->GetOwner())
	{
		APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
		TScriptInterface<IAttackInterface> AttackInterface = TScriptInterface<IAttackInterface>(MeshComp->GetOwner());
		TArray<FHitResult> arResult{};
		FCollisionShape Collision = FCollisionShape::MakeSphere(AttackRadius);
		FCollisionQueryParams Param{};
		Param.AddIgnoredActor(MeshComp->GetOwner());
		FCollisionResponseParams ResponseParam{};
		if (MeshComp->GetWorld()->SweepMultiByChannel(arResult, NewLocation, NewLocation, FQuat::Identity, ECollisionChannel::ECC_Pawn,
			Collision, Param))
		{
			TSet< APawn*> Attacked{};
			for (const FHitResult& Hit : arResult)
			{
				bool bReadldyIn{};
				APawn* Pawn = Cast< APawn>(Hit.GetActor());
				if (!Pawn)
					continue;
				Attacked.FindOrAdd(Pawn, &bReadldyIn);
				if (bReadldyIn)
					continue;
				TScriptInterface<IAttackInterface> OtherAttack = TScriptInterface<IAttackInterface>(Hit.GetActor());
				if (OtherAttack && FGenericTeamId::GetAttitude(Hit.GetActor(), OwnerPawn->GetController()) != ETeamAttitude::Friendly)
				{
					IAttackInterface::Execute_DamagedCharacter(Hit.GetActor(), AttackInterface);
				}

				for (UAttackObject* AttackObject : AttackEventObject)
				{
					if (AttackObject)
					{
						AttackObject->ExecuteAttackEvent(MeshComp, Hit);
					}
				}
			}
		}
	}
#if WITH_EDITOR	
		DrawDebugSphere(pWorld, NewLocation, AttackRadius, 20, FColor::Red, false, 0.5f, 0, 0.5f);
		for (UAttackObject* AttackObject : AttackEventObject)
		{
			if (AttackObject)
			{
				AttackObject->ExecuteDebugAttackEvent(MeshComp);
			}
		}
#endif
}
