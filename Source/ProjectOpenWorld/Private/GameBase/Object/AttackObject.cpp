#include "GameBase/Object/AttackObject.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameBase/Interface/AttackInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GenericTeamAgentInterface.h"

void UAttackObject_KnockBackDirection::AttackEvent(USkeletalMeshComponent* CauserMesh, const FHitResult& HitData) const
{
	AActor* AttackTarget = HitData.GetActor();
	if (AttackTarget)
	{
		FVector Direction = KnockBackDirection;
		Direction.Normalize();
		if (!bIsWorldDirection && CauserMesh)
		{
			Direction = CauserMesh->GetOwner()->GetActorRotation().RotateVector(Direction);
		}
		FVector LaunchVelocity = Direction * KnockBackForce;
		UPrimitiveComponent* TargetRootComponent = Cast<UPrimitiveComponent>(AttackTarget->GetRootComponent());
		if (TargetRootComponent && TargetRootComponent->IsSimulatingPhysics())
		{
			TargetRootComponent->AddImpulse(LaunchVelocity, NAME_None, true);
		}
		else if (ACharacter* TargetCharacter = Cast<ACharacter>(AttackTarget))
		{
			TargetCharacter->LaunchCharacter(LaunchVelocity, true, true);
		}
	}
}

void UAttackObject_KnockBackDirection::DebugAttackEvent(USkeletalMeshComponent* CauserMesh, FVector AttackLocation, float HitRadius) const
{
	if (!CauserMesh || !CauserMesh->GetWorld() || CauserMesh->GetWorld()->HasBegunPlay())
		return;
	FVector Direction = KnockBackDirection;
	Direction.Normalize();
	if (!bIsWorldDirection )
	{
		Direction = FRotator(0,90,0).RotateVector(Direction);
	}
	FVector LaunchVelocity = Direction * KnockBackForce;
	DrawDebugDirectionalArrow(CauserMesh->GetWorld(), CauserMesh->GetOwner()->GetActorLocation() , CauserMesh->GetOwner()->GetActorLocation()+ Direction * 100.f,
		50.f, DebugData.DebugColor, false, DebugData.DebugLifeTime, 0, 5.f);
}

void UAttackObject_PlayerStun::AttackEvent(USkeletalMeshComponent* CauserMesh, const FHitResult& HitData) const
{
	if (ACharacter* TargetCharacter = Cast<ACharacter>(HitData.GetActor()))
	{
		TargetCharacter->DisableInput(nullptr);
		FTimerHandle UnusedHandle;
		FTimerDelegate TimerDel;
		TimerDel.BindLambda([TargetCharacter]()
			{
				TargetCharacter->EnableInput(nullptr);
			});
		TargetCharacter->GetWorld()->GetTimerManager().SetTimer(UnusedHandle, TimerDel, StunTime, false);
	}
}

void UAttackObject_Impulse::AttackEvent(USkeletalMeshComponent* CauserMesh, const FHitResult& HitData) const
//void UAttackObject_Impulse::AttackEvent(USkeletalMeshComponent* AttackCauser, AActor* AttackTarget) const
{
	if (ACharacter* TargetCharacter = Cast<ACharacter>(HitData.GetActor()))
	{
		ACharacter* CauserCharacter = Cast<ACharacter>(CauserMesh->GetOwner());
		USkeletalMeshComponent* MeshComp = CauserCharacter->GetMesh();
		FVector NewLocation = MeshComp->GetSocketLocation(SocketName) + MeshComp->GetComponentRotation().Quaternion() * SocketOffset;

		// 거리 계산
		float Distance = FVector::Dist(TargetCharacter->GetActorLocation(), NewLocation);

		if (AttackRadius > 0)
		{
			// 거리 비율 계산 (Distance / AttackRadius의 제곱)
			float Ratio = FMath::Pow(Distance / AttackRadius, 2.0f);

			// 거리별 힘 계산 (비율을 반영)
			float ForceMultiplier = FMath::Clamp(1.0f - Ratio, 0.0f, 1.0f);

			// 힘 벡터 계산
			FVector LaunchDirection = (TargetCharacter->GetActorLocation() - NewLocation ).GetSafeNormal();
			//LaunchDirection.X *= 3;
			//LaunchDirection.Y *= 3;
			LaunchDirection.Z = abs(LaunchDirection.Z);
			LaunchDirection = LaunchDirection.GetSafeNormal();
			FVector ImpulseForce = LaunchDirection * ForceMultiplier * MaxImpulseForce;
			UE_LOG(LogTemp, Warning, TEXT("Distance: %f, Ratio: %f, ForceMultiplier: %f, ImpulseForce: %s"), Distance, Ratio, ForceMultiplier, *ImpulseForce.ToString());
			// 힘 적용
			TargetCharacter->LaunchCharacter(ImpulseForce, true, true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Distance out of range: %f"), Distance);
		}
	}
}

void UAttackObject_Impulse::DebugAttackEvent(USkeletalMeshComponent* CauserMesh, FVector AttackLocation, float HitRadius) const
{
	if (!CauserMesh || !CauserMesh->GetWorld() || CauserMesh->GetWorld()->HasBegunPlay())
		return;
	FVector NewLocation = CauserMesh->GetSocketLocation(SocketName) + CauserMesh->GetComponentRotation().Quaternion() * SocketOffset;
	DrawDebugSphere(CauserMesh->GetWorld(), NewLocation, AttackRadius, 20, FColor::Purple, false, DebugData.DebugLifeTime, 0, 0.5f);

	FVector Direction = {1,0,0};
	Direction = FRotator(0, 90, 0).RotateVector(Direction);
	DrawDebugDirectionalArrow(CauserMesh->GetWorld(), CauserMesh->GetOwner()->GetActorLocation(), CauserMesh->GetOwner()->GetActorLocation() + Direction * 100.f,
		50.f, DebugData.DebugColor, false, DebugData.DebugLifeTime, 0, 5.f);
}

void UAttackObject_Attack::AttackEvent(USkeletalMeshComponent* CauserMesh, const FHitResult& HitData) const
{
	ACharacter* CauserCharacter = Cast<ACharacter>(CauserMesh->GetOwner());
	if (!CauserCharacter)
		return;
	TScriptInterface<IAttackInterface> OtherAttack = TScriptInterface<IAttackInterface>(HitData.GetActor());
	TScriptInterface<IAttackInterface> AttackInterface = TScriptInterface<IAttackInterface>(CauserCharacter);
	if (OtherAttack)
	{
		IAttackInterface::Execute_DamagedCharacter(HitData.GetActor(), AttackInterface);
	}
}

void UAttackObject_Attack::DebugAttackEvent(USkeletalMeshComponent* CauserMesh, FVector AttackLocation, float HitRadius) const
{
	if (!CauserMesh || !CauserMesh->GetWorld())
		return;
	DrawDebugSphere(CauserMesh->GetWorld(), AttackLocation, HitRadius, 20, DebugData.DebugColor, false, DebugData.DebugLifeTime, 0, 0.5f);
}
