#include "Pal/Character/BossMonster_Anubis.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Pal/Component/PalPatternComponent.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Widget/PatternWidget_Anubis.h"

void ABossMonster_Anubis::BeginPlay()
{
	Super::BeginPlay();
	OnDamagedDelegate.AddUniqueDynamic(this, &ABossMonster_Anubis::OnCustomModeDamaged);
	PatternWidget->SetVisibility(false);
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

	PatternWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PatternWidget"));
	PatternWidget->SetupAttachment(RootComponent);
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
	float HPRadio = GetCurrentHp() / GetMaxHp();
	if (HPRadio <= 0.5f)
	{
		AttackComponent->StopAttack();
		AttackComponent->SetAttackData(ESubAttackType::Skill01);
		AttackComponent->SetAttackTarget(Other);
		AttackComponent->StartAttack();
		PatternComponent->UpdatePatternCondition(0);
	}
}

void ABossMonster_Anubis::StartPatternWidget(int nCount)
{
	PatternWidget->SetVisibility(true);
	FVector2D DrawSize = {64.0f, 64.0f};
	float Count = static_cast<float>(nCount);
	Count += 1;
	PatternWidget->AddRelativeLocation(FVector(0.0f, 64.0f * (Count *0.5f), 200.0f));
	PatternWidget->SetDrawSize(FVector2D(DrawSize.X* nCount, DrawSize.Y));
	if (UPatternWidget_Anubis* PatternWidgetCast = Cast<UPatternWidget_Anubis>(PatternWidget->GetUserWidgetObject()))
	{
		PatternWidgetCast->SetPatternImage(nCount);
	}
}

void ABossMonster_Anubis::UpdatePatternWidget()
{
	if (UPatternWidget_Anubis* PatternWidgetCast = Cast<UPatternWidget_Anubis>(PatternWidget->GetUserWidgetObject()))
	{
		PatternWidgetCast->UpdatePatternImage();
	}
}

void ABossMonster_Anubis::EndPatternWidget()
{
	PatternWidget->SetVisibility(false);
}
