#include "Player/Animation/PlayerAnimInstance.h"
#include "Player/Character/BasePlayer.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "Player/Component/PlayerAnimationComponent.h"
#include "GameFramework/PlayerController.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	OwnerPlayer = Cast<ABasePlayer>(TryGetPawnOwner());
	if (OwnerPlayer)
	{
		AnimationComponent = OwnerPlayer->GetPlayerAnimationComponent();
		//OwnerPlayer->OnStateChangeDelegate.AddUniqueDynamic(this, &UPlayerAnimInstance::SetPlayerState);
	}
	Super::NativeInitializeAnimation();
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwnerPlayer || !MovementComponent || !AnimationComponent)
		return;
	Velocity = MovementComponent->Velocity;

	IsClimbing = AnimationComponent->IsClimbing();
	AnimationState = AnimationComponent->GetAnimationState();
	LeftHandNormal = AnimationComponent->LeftHandNormal();
	RightHandNormal = AnimationComponent->RightHandNormal();
	LeftFootNormal = AnimationComponent->LeftFootNormal();
	RightFootNormal = AnimationComponent->RightFootNormal();

	FVector ForwardVector = OwnerPlayer->GetActorForwardVector();
	float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ForwardVector.GetSafeNormal2D(), Velocity.GetSafeNormal2D())));
	float Cos = FVector::DotProduct(OwnerPlayer->GetActorRightVector().GetSafeNormal2D(), Velocity.GetSafeNormal2D());
	if (Cos < 0)
		Angle = -Angle;

	CurrentPlayerState = OwnerPlayer->GetPlayerState();
	DesireYaw = FMath::Clamp(Angle, -180.0f, 180.0f);
}

void UPlayerAnimInstance::SetPlayerState(EPlayerState NewState)
{
	CurrentPlayerState = NewState;
}
