#include "GameBase/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMetaData.h"
#include "GameBase/Component/CharacterMontageComponent.h"
#include "GameBase/Component/StatComponent.h"
#include "Components/CapsuleComponent.h"


ABaseCharacter::ABaseCharacter() : Super()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Overlap);

	HPStat = CreateDefaultSubobject<UStatComponent>(TEXT("HPStat"));
	AttackStat = CreateDefaultSubobject<UStatComponent>(TEXT("AttackStat"));
	DefendStat = CreateDefaultSubobject<UStatComponent>(TEXT("DefendStat"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//UAnimationAsset* Asset{};
	//Asset->FindMetaDataByClass<UAnimMetaData>();

}

void ABaseCharacter::UseControllerDesiredRotation()
{
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void ABaseCharacter::UseOrientRotationToMovement()
{
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

bool ABaseCharacter::LaunchAttack_Implementation(const TScriptInterface<IAttackInterface>& Other, FVector LaunchVelocity, bool bXYOverride, bool bZOverride)
{
	LaunchCharacter(LaunchVelocity, bXYOverride, bZOverride);
	return true;
}
