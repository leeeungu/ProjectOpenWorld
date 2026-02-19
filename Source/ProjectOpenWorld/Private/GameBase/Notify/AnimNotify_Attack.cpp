#include "GameBase/Notify/AnimNotify_Attack.h"
#include "GameBase/Interface/AttackInterface.h"
#include "Kismet/GameplayStatics.h"
#include "GenericTeamAgentInterface.h"
#include "GameBase/Object/AttackObject.h"

void UAnimNotify_Attack::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent)
{
	Super::OnAnimNotifyCreatedInEditor(ContainingAnimNotifyEvent);
	for (UAttackObject* AttackObject : AttackEventObject)
	{
		if (AttackObject)
		{
			//AttackObject->insti
		}
	}
}

void UAnimNotify_Attack::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

FCollisionShape UAnimNotify_Attack::GetAttackCollisionShape() const
{
	return FCollisionShape();
}

FVector UAnimNotify_Attack::GetStartLocation(USkeletalMeshComponent* MeshComp) const
{
	return MeshComp->GetSocketLocation(NAME_None);
}

FVector UAnimNotify_Attack::GetEndLocation(USkeletalMeshComponent* MeshComp) const
{
	return MeshComp->GetSocketLocation(NAME_None);
}

bool UAnimNotify_Attack::CollisionAttackResult(USkeletalMeshComponent* MeshComp, TArray<FHitResult>& HitResult)
{
	return  MeshComp->GetWorld()->SweepMultiByObjectType(HitResult, GetStartLocation(MeshComp), GetEndLocation(MeshComp)
		, FQuat::Identity, ECollisionChannel::ECC_Pawn,
		GetAttackCollisionShape(), {});
}

void UAnimNotify_Attack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	FVector NewLocation = GetStartLocation(MeshComp);
	UWorld* pWorld = MeshComp->GetWorld();
	FVector EndLocation = GetEndLocation(MeshComp);
	if (MeshComp->GetOwner() && pWorld)
	{
		APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
		TScriptInterface<IAttackInterface> AttackInterface = TScriptInterface<IAttackInterface>(MeshComp->GetOwner());
		TArray<FHitResult> arResult{};
		FCollisionQueryParams Param{};
		FCollisionResponseParams ResponseParam{};
		CollisionAttackResult(MeshComp, arResult);
		TSet< APawn*> Attacked{};
		for (const FHitResult& Hit : arResult)
		{
			bool bReadldyIn{};
			APawn* Pawn = Cast< APawn>(Hit.GetActor());
			if (!Pawn || Pawn == OwnerPawn || !Pawn->Implements<UAttackInterface>() && !Hit.bBlockingHit)
				continue;
			if (FGenericTeamId::GetAttitude(Pawn->GetController(), OwnerPawn->GetController()) != ETeamAttitude::Hostile)
				continue;
			Attacked.FindOrAdd(Pawn, &bReadldyIn);
			if (bReadldyIn)
				continue;
			for (UAttackObject* AttackObject : AttackEventObject)
			{
				if (AttackObject)
				{
					AttackObject->ExecuteAttackEvent(MeshComp, Hit);
				}
			}
		}
		//if (arResult.IsEmpty())
		//{
		//	UE_LOG(LogTemp, Log, TEXT("UAnimNotify_Attack :: Notify Attacked Count : %d"), Attacked.Num());
		//}
	}
	AttackEventObjectDebug(MeshComp);
}

void UAnimNotify_Attack::AttackEventObjectDebug(USkeletalMeshComponent* MeshComp)
{
#if WITH_EDITOR	
	if (!MeshComp->GetWorld()->HasBegunPlay())
	{
		FVector StartLocation = GetStartLocation(MeshComp);
		FVector EndLocation = GetEndLocation(MeshComp);
		for (UAttackObject* AttackObject : AttackEventObject)
		{
			if (AttackObject)
			{
				AttackObject->ExecuteDebugAttackEvent(MeshComp, StartLocation, EndLocation, GetAttackCollisionShape());
			}
		}
	}
#endif
}
