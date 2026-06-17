#include "Pal/Character/PalBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

void APalBaseCharacter::UseControllerDesiredRotation()
{
	if (!GetCharacterMovement())
		return;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}
void APalBaseCharacter::UseOrientRotationToMovement()
{
	if (!GetCharacterMovement())
		return;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}
void APalBaseCharacter::HideCharacter()
{
	bIsHidden = true;
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	StopAnimMontage();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->SetMovementMode(MOVE_None);
}

void APalBaseCharacter::VisibleCharacter()
{
	bIsHidden = false;
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}
