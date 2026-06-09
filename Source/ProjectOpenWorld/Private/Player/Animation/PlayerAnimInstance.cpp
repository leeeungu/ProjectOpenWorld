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

bool UPlayerAnimInstance::StartAnimSection()
{
	if (!StartAnim || !LoopAnim || !EndAnim)
	{
		UE_LOG(LogTemp, Warning, TEXT("Animation sequences are not set!"));
		return false;
	}

	if (CurrentAnimationSection == EAnimationSectionType::None)
	{
		CurrentAnimationSection = EAnimationSectionType::Start;
		OnStartAnimSection.Broadcast();
		return true;
	}
	return false;
}



bool UPlayerAnimInstance::EndAnimSection()
{
	if (CurrentAnimationSection == EAnimationSectionType::Loop || CurrentAnimationSection == EAnimationSectionType::Start)
	{
		CurrentAnimationSection = EAnimationSectionType::End;
		return true;
	}
	return false;
}

bool UPlayerAnimInstance::ResetAnimSection()
{
	if (CurrentAnimationSection != EAnimationSectionType::None)
	{
		return false;
	}
	CurrentAnimationSection = EAnimationSectionType::None;
	SetAnimationSequences(nullptr, nullptr, nullptr);
	return true;
}

void UPlayerAnimInstance::AnimNotify_LoopAnimSection()
{
	if (CurrentAnimationSection == EAnimationSectionType::Start)
	{
		CurrentAnimationSection = EAnimationSectionType::Loop;
		OnStartLoopAnimSection.Broadcast();
	}
}

void UPlayerAnimInstance::AnimNotify_FinishAnimSection()
{
	CurrentAnimationSection = EAnimationSectionType::None;
	OnFinishAnimSection.Broadcast();
}

bool UPlayerAnimInstance::SetAnimationSequences(UAnimSequence* Start, UAnimSequence* Loop, UAnimSequence* End)
{
	if (CurrentAnimationSection == EAnimationSectionType::None)
	{
		StartAnim = Start;
		LoopAnim = Loop;
		EndAnim = End;
		bSetAnimation = nullptr == StartAnim && nullptr == EndAnim && nullptr == LoopAnim;
		return true;
	}
	return false;
}

bool UPlayerAnimInstance::IsSetAnimation() const
{
	return bSetAnimation;
}

bool UPlayerAnimInstance::SetArchitectAnimSequence()
{
	return SetAnimationSequences(ArchitectStartAnim, ArchitectLoopAnim, ArchitectEndAnim);
}

void UPlayerAnimInstance::SetPlayerState(EPlayerState NewState)
{
	CurrentPlayerState = NewState;
}