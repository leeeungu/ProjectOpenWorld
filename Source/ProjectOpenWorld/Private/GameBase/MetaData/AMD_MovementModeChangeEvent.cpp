#include "GameBase/MetaData/AMD_MovementModeChangeEvent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UCharacterMovementComponent* UAMD_MovementModeChangeEvent::GetCharacterMovementComponent(UAnimInstance* Animinstance)
{
	if (!Animinstance)
		return nullptr;
	ACharacter* OwnerCharacter = Cast<ACharacter>(Animinstance->TryGetPawnOwner());
	if (!OwnerCharacter)
		return nullptr;
	return OwnerCharacter->GetCharacterMovement();
}

void UAMD_MovementModeChangeEvent::StartEvent(UAnimInstance* Animinstance)
{
	if(UCharacterMovementComponent* MovementComp = GetCharacterMovementComponent(Animinstance))
		MovementComp->SetMovementMode(StartMovemntMode, StartCustomMovemntMode);
}

void UAMD_MovementModeChangeEvent::EndEvent(UAnimInstance* Animinstance)
{
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovementComponent(Animinstance))
		MovementComp->SetMovementMode(EndMovemntMode, EndCustomMovemntMode);
}
