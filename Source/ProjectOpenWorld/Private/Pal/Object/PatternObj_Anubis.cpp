#include "Pal/Object/PatternObj_Anubis.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/Character/BasePlayer.h"
#include "GenericTeamAgentInterface.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Component/PalMonsterInteractionComponent.h"
#include "Pal/Character/BossMonster.h"

void UPatternObj_Anubis01::InitializePattern(AActor* OwnerActor, AActor* TargetActor)
{
	OwnerCharacter = Cast<ABossMonster>(OwnerActor);
	TargetPlayer = Cast<ABasePlayer>(TargetActor);
}

void UPatternObj_Anubis01::StartPattern()
{
	bool bError = !OwnerCharacter || !TargetPlayer;
	if (!OwnerCharacter)
		UE_LOG(LogTemp, Error, TEXT("UPatternObj_Anubis01::StartPattern OwnerCharacter is nullptr"));
	if (!TargetPlayer)
		UE_LOG(LogTemp, Error, TEXT("UPatternObj_Anubis01::StartPattern TargetPlayer is nullptr"));

	if (bError)
		return;

	OwnerCharacter->UseOrientRotationToMovement();
	PatternMoveDirection = FMath::VRand().GetSafeNormal2D();
	if (TargetPlayer)
	{
		TargetPlayer->ChangePlayerState(EPlayerState::TopDown); 
	}
	CurTime = 0.f;
	CurrentCount = PatternSolveCount;
	bIsPatternActive = true;
}

void UPatternObj_Anubis01::UpdatePattern(float DeltaTime)
{
	if (!bIsPatternActive)
		return;
	CurTime += DeltaTime;
	if (CurTime >= PatternStopTime)
	{
		if (PatternMoveSpeed != 0.f)
		{
			FVector Direction = PatternMoveDirection.GetSafeNormal2D();
			FVector DeltaLocation = Direction * PatternMoveSpeed * DeltaTime;
			FHitResult HitResult{};
			OwnerCharacter->AddActorWorldOffset(DeltaLocation, true, &HitResult);
			if (HitResult.GetActor())
			{
				if (ACharacter* Other = Cast<ACharacter>(HitResult.GetActor()))
				{
					if (FGenericTeamId::GetAttitude(Other->Controller, OwnerCharacter->Controller) == ETeamAttitude::Friendly)
					{
						FVector HitNormal = HitResult.GetActor()->GetActorForwardVector().GetSafeNormal2D();
						PatternMoveDirection = Direction - 2 * FVector::DotProduct(Direction, HitNormal) * HitNormal;
						PatternMoveDirection = PatternMoveDirection.GetSafeNormal2D();
					}
				}
			}
		}
	}
	FRotator CurrentRotation = { 0, PatternRotateSpeedYaw * DeltaTime, 0 };
	if(OwnerCharacter)
		OwnerCharacter->AddActorWorldRotation(CurrentRotation);
}

void UPatternObj_Anubis01::EndPattern()
{
	if (!OwnerCharacter || !TargetPlayer)
		return;
	OwnerCharacter->UseControllerDesiredRotation();
	bIsPatternActive = false;
	if (TargetPlayer)
	{
		TargetPlayer->ChangePlayerState(EPlayerState::Battle);
	}
	if (OwnerCharacter->GetMonsterInteractionComponent())
		OwnerCharacter->GetMonsterInteractionComponent()->EndActiveInteraction();
	OwnerCharacter = nullptr;
	TargetPlayer = nullptr;
}

void UPatternObj_Anubis01::UpdateCondition()
{
	if (!bIsPatternActive)
		return;
	--CurrentCount;
	CurTime = 0.f;
	if (CurrentCount <= 0)
	{
		bIsPatternActive = false;
		if (OwnerCharacter && OwnerCharacter.Get())
		{
			OwnerCharacter->SetStunned(StunDuration);
			if (OwnerCharacter && OwnerCharacter.Get())
			{
				if (OwnerCharacter->GetMonsterInteractionComponent())
					OwnerCharacter->GetMonsterInteractionComponent()->EndActiveInteraction();
			}
			if (OwnerCharacter && OwnerCharacter.Get())
			{
				if (OwnerCharacter->GetAttackComponent())
					OwnerCharacter->GetAttackComponent()->StopAttack();
			}
		}
	}
}
