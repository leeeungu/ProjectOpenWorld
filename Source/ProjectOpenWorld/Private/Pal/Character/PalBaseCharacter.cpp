#include "Pal/Character/PalBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	StopAnimMontage();
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
}

void APalBaseCharacter::VisibleCharacter()
{
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}
