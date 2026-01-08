#include "Pal/Character/BossMonster_Anubis.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Pal/Component/PalPatternComponent.h"

void ABossMonster_Anubis::BeginPlay()
{
	Super::BeginPlay();
	OnDamagedDelegate.AddUniqueDynamic(this, &ABossMonster_Anubis::OnCustomModeDamaged);
}

ABossMonster_Anubis::ABossMonster_Anubis() : ABossMonster()
{
	PrimaryActorTick.bCanEverTick = true;
	// Script / Engine.SkeletalMesh'/Game/Pal/Model/Monster/Anubis/Mesh/SK_Anubis.SK_Anubis'
	GetCapsuleComponent()->SetCapsuleHalfHeight(150.f);
	GetCapsuleComponent()->SetCapsuleRadius(80.0f);
	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Pal/Model/Monster/Anubis/Mesh/SK_Anubis.SK_Anubis"));
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -150.f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate.Yaw = 540.0f;
	//Script/Engine.AnimBlueprint'/Game/Pal/Model/Monster/Anubis/Boss/ABP_Anubis_Boss.ABP_Anubis_Boss'
	ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPAsset(TEXT("/Game/Pal/Model/Monster/Anubis/Boss/ABP_Anubis_Boss.ABP_Anubis_Boss_C"));
	if (AnimBPAsset.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimBPAsset.Class);
	}
}

void ABossMonster_Anubis::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABossMonster_Anubis::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
}

void ABossMonster_Anubis::OnCustomModeDamaged(AActor* Other, float Damaage)
{
	PatternComponent->UpdatePatternCondition(0);
}
