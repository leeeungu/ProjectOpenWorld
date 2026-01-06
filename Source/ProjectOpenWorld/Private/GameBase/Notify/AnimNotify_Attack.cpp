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


	FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UAnimNotify_Attack, bInstanceObject))
	{
		if (bInstanceObject)
		{
			bInstanceObject = false;
			for (int32 i = 0; i < 8; ++i)
			{
				EAttackObjectUtill Utill = static_cast<EAttackObjectUtill>(1 << i);
				if (EnumHasAnyFlags(static_cast<EAttackObjectUtill>(ObjectUtillMask), Utill))
				{
					TObjectPtr<UAttackObject> NewAttackObject = GetAttackObjectByUtill(Utill);
					if (NewAttackObject)
					{
						AttackEventObject.Add(NewAttackObject);
					}
				}
			}
			ObjectUtillMask = 0;
		}
	}
}

TObjectPtr<UAttackObject> UAnimNotify_Attack::GetAttackObjectByUtill(EAttackObjectUtill Utill) const
{
	const UClass* AttackObjectClass = nullptr;
	switch (Utill)
	{
	case EAttackObjectUtill::Attack:
		AttackObjectClass = UAttackObject_Attack::StaticClass();
		break;
	case EAttackObjectUtill::KnockBackDirection:
		AttackObjectClass = UAttackObject_KnockBackDirection::StaticClass();
		break;
	case EAttackObjectUtill::PlayerStun:
		AttackObjectClass = UAttackObject_PlayerStun::StaticClass();
		break;
	case EAttackObjectUtill::Impulse:
		AttackObjectClass = UAttackObject_Impulse::StaticClass();
		break;
	default:
		break;
	}
	if (AttackObjectClass)
		return NewObject< UAttackObject>(const_cast<UAnimNotify_Attack*>(this), AttackObjectClass);
	return nullptr;
}

void UAnimNotify_Attack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	FVector NewLocation = MeshComp->GetSocketLocation(SocketName) + MeshComp->GetComponentRotation().Quaternion() * SocketOffset;
	UWorld* pWorld = MeshComp->GetWorld();
	if (MeshComp->GetOwner() && pWorld)
	{
		APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
		TScriptInterface<IAttackInterface> AttackInterface = TScriptInterface<IAttackInterface>(MeshComp->GetOwner());
		TArray<FHitResult> arResult{};
		FCollisionShape Collision = FCollisionShape::MakeSphere(AttackRadius);
		FCollisionQueryParams Param{};
		Param.AddIgnoredActor(MeshComp->GetOwner());
		FCollisionResponseParams ResponseParam{};
		if (pWorld->SweepMultiByChannel(arResult, NewLocation, NewLocation, FQuat::Identity, ECollisionChannel::ECC_Pawn,
			Collision, Param))
		{
			TSet< APawn*> Attacked{};
			for (const FHitResult& Hit : arResult)
			{
				bool bReadldyIn{};
				APawn* Pawn = Cast< APawn>(Hit.GetActor());
				if (!Pawn || Pawn == OwnerPawn)
					continue;
				UE_LOG(LogTemp, Warning, TEXT("Hit Pawn : %s"), *Pawn->GetName());
				UE_LOG(LogTemp, Warning, TEXT("Owner Pawn : %s"), *OwnerPawn->GetName());
				UE_LOG(LogTemp, Warning, TEXT("%d %d"), (uint8)Cast<IGenericTeamAgentInterface>(Pawn->GetController())->GetGenericTeamId(),
					(uint8)Cast<IGenericTeamAgentInterface>(OwnerPawn->GetController())->GetGenericTeamId());
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
		}
	}
#if WITH_EDITOR	
	for (UAttackObject* AttackObject : AttackEventObject)
	{
		if (AttackObject)
		{
			AttackObject->ExecuteDebugAttackEvent(MeshComp, NewLocation, AttackRadius);
		}
	}
#endif
}
