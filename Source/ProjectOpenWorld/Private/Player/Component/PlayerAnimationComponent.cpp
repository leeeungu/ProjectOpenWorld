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
	{
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
	}
	if (bClimbing)
	{
		{
			// 바닥 체크
			FVector Start = OwnerCharacter->GetActorLocation();
			FVector End = OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorUpVector() * -98.0f;
			FHitResult rHit{};
			if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End,
				UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), true, IgnoreArray, EDrawDebugTrace::ForOneFrame, rHit, true))
			{
				float TargetAngle = OwnerCharacter->GetCharacterMovement()->GetWalkableFloorAngle();
				float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FVector::UpVector, rHit.ImpactNormal)));
				if (Angle < TargetAngle)
				{
					bCanClimb = false;
					AllHit = false;
				}
			}
		}
		{
			bEmpthyUp = false;
			FVector Start = OwnerCharacter->GetMesh()->GetSocketLocation(ClimbData.Head) + OwnerCharacter->GetActorUpVector() * 20.0f
				+ OwnerCharacter->GetActorForwardVector() * -60.0f;
			FVector End = Start + OwnerCharacter->GetActorForwardVector() * 120.0f;
			FHitResult rHit{};
			if (!UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End,
				UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), true, IgnoreArray, EDrawDebugTrace::ForOneFrame, rHit, true))
			{
				FVector CapStart = 
					OwnerCharacter->GetMesh()->GetSocketLocation(ClimbData.Head) //+ OwnerCharacter->GetActorUpVector() * 50.0f
					+ OwnerCharacter->GetActorForwardVector() * 60.0f;
					//rHit.TraceEnd;// +OwnerCharacter->GetActorForwardVector() * 40.0f;
				FVector CapEnd = CapStart + FVector::UpVector * OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
				FHitResult CapHit{};
				bEmpthyUp = !UKismetSystemLibrary::SphereTraceSingle(GetWorld(), CapStart, CapEnd, OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius() * 0.8f,
					UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), true, IgnoreArray, EDrawDebugTrace::ForOneFrame, CapHit, true);
					//0,FLinearColor::Red,FLinearColor::Blue, 2.0f);
			}

		}
		{

			float TargetAngle = OwnerCharacter->GetCharacterMovement()->GetWalkableFloorAngle();
			float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FVector::UpVector, GetAVGNormal())));
			if (Angle < TargetAngle - 10)
			{
				AllHit = false;
			}
		}
	}
	return AllHit;
}

void UPlayerAnimationComponent::StartArchitecture()
{
	AnimationState = EAnimationState::Architecture;
}

void UPlayerAnimationComponent::ResetAnimationState()
{
	AnimationState = EAnimationState::Travel;
}

void UPlayerAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*const FHitResult* Hit = GetPelvisHit();
	FVector Avg = GetAVGPosition();
	FVector Normal = GetAVGNormal();
	float Dis = FVector::Distance(OwnerCharacter->GetActorLocation(), Avg);
	if (Dis < 65.0f)
	{
		OwnerCharacter->AddActorWorldOffset(Hit->ImpactNormal * OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius() * GetWorld()->GetDeltaSeconds() * 2.f);
	}
	else if (Dis > 67.0f)
	{
		OwnerCharacter->AddActorWorldOffset(Hit->ImpactNormal * -OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius() * GetWorld()->GetDeltaSeconds() * 2.0f);
	}*/

}

bool UPlayerAnimationComponent::StartClimb()
{
	if (!OwnerCharacter || !ClimbLineCheck() ) // 시작 시 climb end 중에 다시 climb 되는 문제가 있음
	{
		return false;
	}
	bClimbing = true;
	SetComponentTickEnabled(true);
	// 벽에 붙어서 climb 하면 벽 안에 들어가 버려서 떨어 뜨릴려고 수정했는데
	// 벽과 비스듬하면 여전히 벽에 뭍히는 버그가 있음 (ray를 쏴야할려나, 계산하고 tick에서 보정을 칠까 )
	if (ClimbData.arrHitResult[SClimbRayData::ERoot].Distance < OwnerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius() * 2.8)
	{
	OwnerCharacter->AddActorWorldOffset(ClimbData.arrHitResult[SClimbRayData::ERoot].ImpactNormal * 
		GetWorld()->GetDeltaSeconds()* ClimbData.arrHitResult[SClimbRayData::ERoot].Distance*20);
	}
	FRotator Rotation = OwnerCharacter->GetActorRotation();
	Rotation.Roll = 0.0f;
	FRotator NewRotation = (GetCenterNoraml() * -1).Rotation();
	NewRotation.Roll = 0.0f;
	OwnerCharacter->SetActorRotation(NewRotation);
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
	SetComponentTickEnabled(false);
	bClimbing = false;
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

FVector UPlayerAnimationComponent::GetAVGNormal() const
{
	if (ClimbData.arrHitResult[SClimbRayData::ERoot].bBlockingHit == false)
		return FVector::UpVector;
	FVector Normal{};
	const FHitResult* ArHits[4] = {
		&ClimbData.arrHitResult[SClimbRayData::ELHand],
		&ClimbData.arrHitResult[SClimbRayData::ERHand],
		&ClimbData.arrHitResult[SClimbRayData::ELFoot],
		&ClimbData.arrHitResult[SClimbRayData::ERFoot]
	};
	for (const FHitResult* Hit : ArHits)
	{
		if (Hit->bBlockingHit)
		{
			Normal += Hit->ImpactNormal;
		}
	}
	return Normal.GetSafeNormal();
}
