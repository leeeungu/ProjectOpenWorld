#include "GameBase/Notify/SpawnProjectileAttack.h"
#include "GameBase/Actor/ProjectileAttack.h"


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

		//FVector Direction = Params.SpawnRotation.RotateVector(Params.ProjectileDirection);
	//if (!Params.bUseMeshRotation && MeshComp->GetOwner())
	//{
	//	Direction = MeshComp->GetOwner()->GetActorRotation().RotateVector(LaunchParams.ProjectileDirection);
	//}
		if (ProjectileAttack)
		{
			ProjectileAttack->SetActorScale3D(Scale);
			ProjectileAttack->SetActorRotation(SpawnRotation);
			ProjectileAttack->Launch(LaunchParams);
		}

	}
}
