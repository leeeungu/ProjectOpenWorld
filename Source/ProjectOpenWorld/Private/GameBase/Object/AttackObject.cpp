#include "GameBase/Object/AttackObject.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameBase/Interface/AttackInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GenericTeamAgentInterface.h"

void UAttackObject_KnockBackDirection::AttackEvent(USkeletalMeshComponent* CauserMesh, const FHitResult& HitData) const
{
	// Character를 넉백 시키는 함수
	AActor* AttackTarget = HitData.GetActor();
	if (AttackTarget)
	{
		ACharacter* CauserCharacter = Cast<ACharacter>(CauserMesh->GetOwner());
		if (!CauserCharacter)
			return;
		TScriptInterface<IAttackInterface> OtherAttack = TScriptInterface<IAttackInterface>(HitData.GetActor());
		TScriptInterface<IAttackInterface> AttackInterface = TScriptInterface<IAttackInterface>(CauserCharacter);
		if (OtherAttack && IAttackInterface::Execute_KnockBackAttack(HitData.GetActor(), AttackInterface))
		{
			return;
		}
		FVector Direction = KnockBackDirection;
		Direction.Normalize();
		if (!bIsWorldDirection && CauserMesh)
		{
			Direction = CauserMesh->GetOwner()->GetActorRotation().RotateVector(Direction);
		}
		FVector LaunchVelocity = Direction * KnockBackForce;
		UPrimitiveComponent* TargetRootComponent = Cast<UPrimitiveComponent>(AttackTarget->GetRootComponent());

		IAttackInterface::Execute_LaunchAttack(HitData.GetActor(), AttackInterface, LaunchVelocity, true,  true);
	}
}

void UAttackObject_KnockBackDirection::DebugAttackEvent(USkeletalMeshComponent* CauserMesh, FVector StartLocation, FVector EndLocation, const FCollisionShape& CollisionShape) const
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
			TScriptInterface<IAttackInterface> OtherAttack = TScriptInterface<IAttackInterface>(HitData.GetActor());
			TScriptInterface<IAttackInterface> AttackInterface = TScriptInterface<IAttackInterface>(CauserCharacter);
			if (OtherAttack)
			{
				IAttackInterface::Execute_LaunchAttack(HitData.GetActor(), AttackInterface, ImpulseForce, true, true);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Distance out of range: %f"), Distance);
		}
	}
}

void UAttackObject_Impulse::DebugAttackEvent(USkeletalMeshComponent* CauserMesh, FVector StartLocation, FVector EndLocation, const FCollisionShape& CollisionShape) const
{
	if (!CauserMesh || !CauserMesh->GetWorld()) // || CauserMesh->GetWorld()->HasBegunPlay())
		return;
	UE_LOG(LogTemp, Warning, TEXT("UAttackObject_Impulse::DebugAttackEvent"));
	FVector NewLocation = CauserMesh->GetSocketLocation(SocketName) + CauserMesh->GetComponentRotation().Quaternion() * SocketOffset;
	DrawDebugSphere(CauserMesh->GetWorld(), NewLocation, AttackRadius, 20, FColor::Purple, false, DebugData.DebugLifeTime, 0, 0.5f);

	FVector Direction = {1,0,0};
	Direction = FRotator(0, 90, 0).RotateVector(Direction);
	DrawDebugDirectionalArrow(CauserMesh->GetWorld(), CauserMesh->GetOwner()->GetActorLocation(), CauserMesh->GetOwner()->GetActorLocation() + Direction * 100.f,
		50.f, DebugData.DebugColor, false, DebugData.DebugLifeTime, 0, 5.f);
}

void UAttackObject_Attack::AttackEvent(USkeletalMeshComponent* CauserMesh, const FHitResult& HitData) const
{
	// Character에 스턴을 주는 함수
	ACharacter* CauserCharacter = Cast<ACharacter>(CauserMesh->GetOwner());
	if (!CauserCharacter)
		return;
	TScriptInterface<IAttackInterface> OtherAttack = TScriptInterface<IAttackInterface>(HitData.GetActor());
	TScriptInterface<IAttackInterface> AttackInterface = TScriptInterface<IAttackInterface>(CauserCharacter);
	if (OtherAttack)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAttackObject_Attack::AttackEvent - DamagedCharacter"));
		IAttackInterface::Execute_DamagedCharacter(HitData.GetActor(), AttackInterface);
	}
}

void UAttackObject_Attack::DebugAttackEvent(USkeletalMeshComponent* CauserMesh, FVector StartLocation, FVector EndLocation, const FCollisionShape& CollisionShape) const
{
	if (!CauserMesh || !CauserMesh->GetWorld())
		return;
	if (CollisionShape.IsSphere())
	{
		DrawDebugSphere(CauserMesh->GetWorld(), StartLocation, CollisionShape.GetSphereRadius(), 20, DebugData.DebugColor, false, DebugData.DebugLifeTime, 0, 0.5f);
		if(StartLocation != EndLocation)
			DrawDebugSphere(CauserMesh->GetWorld(), EndLocation, CollisionShape.GetSphereRadius(), 20, DebugData.DebugColor, false, DebugData.DebugLifeTime, 0, 0.5f);
	}
	else if (CollisionShape.IsBox())
	{
		DrawDebugBox(CauserMesh->GetWorld(), StartLocation, CollisionShape.GetBox(), DebugData.DebugColor, false, DebugData.DebugLifeTime, 0, 0.5f);
		if (StartLocation != EndLocation)
			DrawDebugBox(CauserMesh->GetWorld(), EndLocation, CollisionShape.GetBox(), DebugData.DebugColor, false, DebugData.DebugLifeTime, 0, 0.5f);
	}
	else if (CollisionShape.IsCapsule())
	{
		DrawDebugCapsule(CauserMesh->GetWorld(), StartLocation, CollisionShape.GetCapsuleHalfHeight(), CollisionShape.GetCapsuleRadius(),
			FQuat::Identity, DebugData.DebugColor, false, DebugData.DebugLifeTime, 0, 0.5f);
		if (StartLocation != EndLocation)
			DrawDebugCapsule(CauserMesh->GetWorld(), EndLocation, CollisionShape.GetCapsuleHalfHeight(), CollisionShape.GetCapsuleRadius(),
				FQuat::Identity, DebugData.DebugColor, false, DebugData.DebugLifeTime, 0, 0.5f);
	}
}

void UAttackObject_HitReact::AttackEvent(USkeletalMeshComponent* CauserMesh, const FHitResult& HitData) const
{
	ACharacter* CauserCharacter = Cast<ACharacter>(CauserMesh->GetOwner());
	if (!CauserCharacter)
		return;
	TScriptInterface<IAttackInterface> OtherAttack = TScriptInterface<IAttackInterface>(HitData.GetActor());
	TScriptInterface<IAttackInterface> AttackInterface = TScriptInterface<IAttackInterface>(CauserCharacter);
	if (OtherAttack)
	{
		IAttackInterface::Execute_HitReaction(HitData.GetActor(), AttackInterface);
	}
}
