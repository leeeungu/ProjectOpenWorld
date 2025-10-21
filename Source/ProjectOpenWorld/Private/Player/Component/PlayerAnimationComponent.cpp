#include "Player/Component/PlayerAnimationComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

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
		i++;
	}
	return bCanClimb;
}

void UPlayerAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//if (ClimbTick(DeltaTime))
	{
		/*if (!bCanClimbDown && EAnimationState::Climb == AnimationState)
		{
			StartTravel();
		}
		*/
		if (EAnimationState::Climb == AnimationState)
		{
			
			
			
			
			
		}

	}
	//if (EAnimationState::Climb == AnimationState)
	//{
	//	StartTravel();
	//}
}

bool UPlayerAnimationComponent::StartClimb()
{
	if (!OwnerCharacter || !ClimbLineCheck())
	{
		return false;
	}
	//OwnerCharacter->SetActorLocation(ClimbForwardHit.Location + ClimbForwardHit.Normal * OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius());
	AnimationState = EAnimationState::Climb;
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Custom);
	OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	OwnerCharacter->GetCharacterMovement()->GravityScale = 0.0f;
	//OwnerCharacter->SetActorRotation((ClimbForwardHit.Normal * -1).Rotation());
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
	//OwnerCharacter->GetCharacterMovement()->SetWalkableFloorAngle(44.765083);
	FRotator NewRotation = OwnerCharacter->GetActorRotation();
	NewRotation.Roll = 0.0F;
	NewRotation.Pitch = 0.0F;
	OwnerCharacter->SetActorRotation(NewRotation);
	// PlayerMoveFunc = &ABasePlayer::MoveTravel;
	return true;
}

FVector UPlayerAnimationComponent::GetLeftNoraml() const
{
	const FHitResult* ArHits[4] =
	{
		&ClimbData.arrHitResult[SClimbRayData::ELHNext],
		&ClimbData.arrHitResult[SClimbRayData::ERHand],
		&ClimbData.arrHitResult[SClimbRayData::ELFNext],
		&ClimbData.arrHitResult[SClimbRayData::ERFoot]
	};

	FVector P1 = ArHits[0]->ImpactPoint;
	FVector P2 = ArHits[1]->ImpactPoint;
	FVector P3 = ArHits[2]->ImpactPoint;

	FVector V1 = P2 - P1;
	FVector V2 = P3 - P1;

	FVector Normal = FVector::CrossProduct(V1, V2);

	return Normal.GetSafeNormal();
}

FVector UPlayerAnimationComponent::GetCenterNoraml() const
{
	const FHitResult* ArHits[4] = {
		&ClimbData.arrHitResult[SClimbRayData::ELHand],
		&ClimbData.arrHitResult[SClimbRayData::ERHand],
		&ClimbData.arrHitResult[SClimbRayData::ELFoot],
		&ClimbData.arrHitResult[SClimbRayData::ERFoot]
	};

	FVector P1 = ArHits[0]->ImpactPoint;
	FVector P2 = ArHits[1]->ImpactPoint;
	FVector P3 = ArHits[2]->ImpactPoint;

	FVector V1 = P2 - P1;
	FVector V2 = P3 - P1;

	FVector Normal = FVector::CrossProduct(V1, V2);

	return Normal.GetSafeNormal();
}

FVector UPlayerAnimationComponent::GetRightNoraml() const
{
	const FHitResult* ArHits[4] = {
		&ClimbData.arrHitResult[SClimbRayData::ERHNext],
		&ClimbData.arrHitResult[SClimbRayData::ELHand],
		&ClimbData.arrHitResult[SClimbRayData::ERFNext],
		&ClimbData.arrHitResult[SClimbRayData::ELFoot]
	};
	FVector P1 = ArHits[0]->ImpactPoint;
	FVector P2 = ArHits[1]->ImpactPoint;
	FVector P3 = ArHits[2]->ImpactPoint;

	FVector V1 = P2 - P1;
	FVector V2 = P3 - P1;

	FVector Normal = FVector::CrossProduct(V1, V2);

	return -Normal.GetSafeNormal();
}

FVector UPlayerAnimationComponent::GetAVGPosition() const
{
	//const FHitResult* ArHits[4] = {
	//	&ClimbData.arrHitResult[SClimbRayData::ELHand],
	//	&ClimbData.arrHitResult[SClimbRayData::ERHand],
	//	&ClimbData.arrHitResult[SClimbRayData::ELFoot],
	//	&ClimbData.arrHitResult[SClimbRayData::ERFoot]
	//};
	if (ClimbData.arrHitResult[SClimbRayData::ERoot].bBlockingHit == false)
		return OwnerCharacter->GetActorLocation();
	FVector Position  = ClimbData.arrHitResult[SClimbRayData::ERoot].ImpactPoint + 
		ClimbData.arrHitResult[SClimbRayData::ERoot].ImpactNormal * OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	//int Count{ };
	//for (const FHitResult* Hit : ArHits)
	//{
	//	if (Hit->bBlockingHit)
	//	{
	//		Position += Hit->ImpactPoint;
	//		Count++;
	//	}
	//}
	return Position ;
}

//void UPlayerAnimationComponent::MoveClimb(const FInputActionValue& Value)
//{
//	FVector2D MovementVector = Value.Get<FVector2D>();
//	if (!OwnerCharacter)
//		return;
//	FVector Right = OwnerCharacter->GetActorRightVector();
//	FVector Up = OwnerCharacter->GetActorUpVector();
//	FVector MoveDir = (Right * MovementVector.X + Up * MovementVector.Y).GetSafeNormal();
//	OwnerCharacter->AddActorWorldOffset(MoveDir * 300.0f * GetWorld()->GetDeltaSeconds(), false);
//}



////////////////////////////////////////////// v1/////////////////////////////////////////////

//#include "Player/Component/PlayerAnimationComponent.h"
//#include "GameFramework/Character.h"
//#include "GameFramework/CharacterMovementComponent.h"
//#include "Kismet/KismetMathLibrary.h"
//#include "Components/CapsuleComponent.h"
//#include "Kismet/KismetSystemLibrary.h"
//
//UPlayerAnimationComponent::UPlayerAnimationComponent()
//{
//	PrimaryComponentTick.bCanEverTick = true;
//}
//
//void UPlayerAnimationComponent::BeginPlay()
//{
//	Super::BeginPlay();
//	OwnerCharacter = Cast<ACharacter>(GetOwner());
//	IgnoreArray.Reset(1);
//	IgnoreArray.Add(OwnerCharacter);
//}
//
//bool UPlayerAnimationComponent::ClimbTick(float DeltaTime)
//{
//	bCanClimb = false;
//	bHeadHit = false;
//	if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorUpVector() * 60, OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorUpVector() * 60 +OwnerCharacter->GetActorForwardVector() * 100,
//		30.0f,
//		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), true, IgnoreArray, EDrawDebugTrace::ForOneFrame, ClimbHeadHit, true))
//	{
//		bCanClimb = true;
//		bHeadHit = true;
//	}
//	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * 150,
//		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), true, IgnoreArray, EDrawDebugTrace::ForOneFrame, ClimbForwardHit, true))
//	{
//		bCanClimb = true;
//		bCanClimbDown = true;
//		if (bHeadHit && ClimbHeadHit.GetActor() != ClimbForwardHit.GetActor())
//		{
//			ClimbForwardHit = ClimbHeadHit;
//		}
//		if(UKismetSystemLibrary::LineTraceSingle(GetWorld(), OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorUpVector() * -70,
//			UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), true, IgnoreArray, EDrawDebugTrace::ForOneFrame, ClimbDownHit, true))
//		{
//			
//			if (FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(OwnerCharacter->GetActorUpVector(), ClimbDownHit.ImpactNormal)))  < 30.f)
//			{
//				bCanClimbDown = false;
//			}
//		}
//		bCanClimbUp = true;
//		if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorUpVector() * +90,
//			UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), true, IgnoreArray, EDrawDebugTrace::ForOneFrame, ClimbUpHit, true))
//		{
//			bCanClimbUp = false;
//		}
//	}
//	else if (bCanClimb && EAnimationState::Climb != AnimationState)
//	{
//		ClimbForwardHit = ClimbHeadHit;
//	}
//	return bCanClimb;
//}
//
//void UPlayerAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//	if (ClimbTick(DeltaTime))
//	{
//		if (!bCanClimbDown && EAnimationState::Climb == AnimationState)
//		{
//			StartTravel();
//		}
//		if (EAnimationState::Climb == AnimationState)
//		{
//			FRotator Rotation = OwnerCharacter->GetActorRotation();
//			Rotation.Roll = 0.0f;
//			
//				
//			FRotator NewRotation = ((ClimbForwardHit.ImpactNormal + ClimbHeadHit.ImpactNormal).GetUnsafeNormal() * -1).Rotation();
//			NewRotation.Roll = 0.0f;
//			OwnerCharacter->SetActorRotation(FMath::RInterpTo(Rotation, NewRotation, DeltaTime, 20.0f));
//		}
//		
//	}
//	else if (EAnimationState::Climb == AnimationState)
//	{
//		StartTravel();
//	}
//}
//
//bool UPlayerAnimationComponent::StartClimb()
//{
//	if(!OwnerCharacter || !bCanClimb)
//	{
//		return false;
//	}
//	OwnerCharacter->SetActorLocation(ClimbForwardHit.Location + ClimbForwardHit.Normal* OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius());
//	AnimationState = EAnimationState::Climb;
//	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Custom);
//	OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
//	OwnerCharacter->GetCharacterMovement()->GravityScale = 0.0f;
//	OwnerCharacter->SetActorRotation((ClimbForwardHit.Normal * -1).Rotation());
//	return true;
//}
//
//bool UPlayerAnimationComponent::StartTravel()
//{
//	if (!OwnerCharacter)
//	{
//		return false;
//	}
//	AnimationState = EAnimationState::Travel;
//	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
//	OwnerCharacter->GetCharacterMovement()->GravityScale = 1.7f;
//	OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
//	//OwnerCharacter->GetCharacterMovement()->SetWalkableFloorAngle(44.765083);
//	FRotator NewRotation = OwnerCharacter->GetActorRotation();
//	NewRotation.Roll = 0.0F;
//	NewRotation.Pitch = 0.0F;
//	OwnerCharacter->SetActorRotation(NewRotation);
//// PlayerMoveFunc = &ABasePlayer::MoveTravel;
//	return true;
//}
//
////void UPlayerAnimationComponent::MoveClimb(const FInputActionValue& Value)
////{
////	FVector2D MovementVector = Value.Get<FVector2D>();
////	if (!OwnerCharacter)
////		return;
////	FVector Right = OwnerCharacter->GetActorRightVector();
////	FVector Up = OwnerCharacter->GetActorUpVector();
////	FVector MoveDir = (Right * MovementVector.X + Up * MovementVector.Y).GetSafeNormal();
////	OwnerCharacter->AddActorWorldOffset(MoveDir * 300.0f * GetWorld()->GetDeltaSeconds(), false);
////}