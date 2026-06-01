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