#include "Player/Component/PlayerAnimationComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"


UPlayerAnimationComponent::UPlayerAnimationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerAnimationComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	IgnoreArray.Reset(1);
	IgnoreArray.Add(OwnerCharacter);
}

bool UPlayerAnimationComponent::ClimbLineCheck()
{
	bCanClimb = false;
	bool AllHit = true;
	FVector ForwardVector = OwnerCharacter->GetActorForwardVector();
	int i{};
	for (const FName* Socket : ClimbData.arrSocekt)
	{
		FVector SocketLocation = OwnerCharacter->GetActorLocation();
		if (Socket)
			SocketLocation = OwnerCharacter->GetMesh()->GetSocketLocation(*Socket);
		FVector Start = SocketLocation - ForwardVector * ClimbData.StartOffset;
		FVector End = SocketLocation + ForwardVector * ClimbData.EndOffset;
		FHitResult& rHit = ClimbData.arrHitResult[i];
		if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End,
			UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), true, IgnoreArray, EDrawDebugTrace::ForOneFrame, rHit, true))
		{
			bCanClimb = true;
		}
		else
		{
			AllHit = false;
		}
		i++;
		if (i >= 6)
			break;
	}
	{
		FVector Start = OwnerCharacter->GetActorLocation();
		FVector End = OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorUpVector() * -90.0f;
		FHitResult rHit{};
		if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End,
			UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), true, IgnoreArray, EDrawDebugTrace::ForOneFrame, rHit, true))
		{
			bCanClimb = false;
			AllHit = false;
		}
	}
	return AllHit;
}

void UPlayerAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UPlayerAnimationComponent::StartClimb()
{
	if (!OwnerCharacter || !ClimbLineCheck())
	{
		return false;
	}
	OwnerCharacter->SetActorLocation(ClimbData.arrHitResult[SClimbRayData::ERoot].Location + ClimbData.arrHitResult[SClimbRayData::ERoot].Normal * OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius());
	AnimationState = EAnimationState::Climb;
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Custom);
	OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	OwnerCharacter->GetCharacterMovement()->GravityScale = 0.0f;
	return true;
}

bool UPlayerAnimationComponent::StartTravel()
{
	if (!OwnerCharacter)
	{
		return false;
	}
	AnimationState = EAnimationState::Travel;
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	OwnerCharacter->GetCharacterMovement()->GravityScale = 1.7f;
	OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
	FRotator NewRotation = OwnerCharacter->GetActorRotation();
	NewRotation.Roll = 0.0F;
	NewRotation.Pitch = 0.0F;
	OwnerCharacter->SetActorRotation(NewRotation);
	return true;
}

FVector UPlayerAnimationComponent::LeftHandNormal() const
{
	return ClimbData.arrHitResult[SClimbRayData::ELHand].ImpactNormal;
}

FVector UPlayerAnimationComponent::RightHandNormal() const
{
	return ClimbData.arrHitResult[SClimbRayData::ERHand].ImpactNormal;
}

FVector UPlayerAnimationComponent::LeftFootNormal() const
{
	return ClimbData.arrHitResult[SClimbRayData::ELFoot].ImpactNormal;
}

FVector UPlayerAnimationComponent::RightFootNormal() const
{
	return ClimbData.arrHitResult[SClimbRayData::ERFoot].ImpactNormal;
}

FVector UPlayerAnimationComponent::GetCenterNoraml() const
{
	return ClimbData.arrHitResult[SClimbRayData::ERoot].ImpactNormal;
}

FVector UPlayerAnimationComponent::GetAVGPosition() const
{
	
	if (ClimbData.arrHitResult[SClimbRayData::ERoot].bBlockingHit == false)
		return OwnerCharacter->GetActorLocation();
	FVector Position{};
	const FHitResult* ArHits[4] = {
		&ClimbData.arrHitResult[SClimbRayData::ELHand],
		&ClimbData.arrHitResult[SClimbRayData::ERHand],
		&ClimbData.arrHitResult[SClimbRayData::ELFoot],
		&ClimbData.arrHitResult[SClimbRayData::ERFoot]
	};
	int Count{ };
	for (const FHitResult* Hit : ArHits)
	{
		if (Hit->bBlockingHit)
		{
			Position += Hit->ImpactPoint;
			Count++;
		}
	}
	return Position / Count;
}