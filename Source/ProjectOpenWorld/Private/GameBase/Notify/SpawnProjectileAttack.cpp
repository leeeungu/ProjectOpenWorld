#include "GameBase/Notify/SpawnProjectileAttack.h"
#include "GameBase/Actor/ProjectileAttack.h"
#include "GameBase/Interface/AttackInterface.h"

void USpawnProjectileAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if ( MeshComp->GetWorld() && AttackProjectile)
	{
		FVector SpawnLocation = MeshComp->GetSocketLocation(SocketName) + LocationOffset;
		FRotator SpawnRotation = MeshComp->GetSocketRotation(SocketName) + RotationOffset;


		FActorSpawnParameters SpawnParams{};
		SpawnParams.Owner = MeshComp->GetOwner();
		SpawnParams.Instigator = Cast<APawn>(MeshComp->GetOwner());
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AProjectileAttack* ProjectileAttack = Cast<AProjectileAttack>(MeshComp->GetWorld()->SpawnActor(AttackProjectile.Get(), &SpawnLocation, &SpawnRotation, SpawnParams));
		if (ProjectileAttack)
		{
			ProjectileAttack->SetActorScale3D(Scale);
			ProjectileAttack->SetActorRotation(SpawnRotation);
			FVector Direction = SpawnRotation.RotateVector(ProjectileDirection);
			if(!bUseMeshRotation && MeshComp->GetOwner())
			{
				Direction = MeshComp->GetOwner()->GetActorRotation().RotateVector(ProjectileDirection);
			}
			ProjectileAttack->InitializeProjectile(MeshComp->GetOwner(), Direction, ProjectileSpeed, ProjectileLifeTime, ProjectileDistanceTraveled);
		}
	}
}
