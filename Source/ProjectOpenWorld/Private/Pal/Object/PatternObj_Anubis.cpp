#include "Pal/Object/PatternObj_Anubis.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/Character/BasePlayer.h"
#include "GenericTeamAgentInterface.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Component/PalMonsterInteractionComponent.h"
#include "Pal/Character/BossMonster_Anubis.h"


void UPatternObj_Anubis01::InitializePattern(AActor* OwnerActor, AActor* TargetActor)
{
	OwnerCharacter = Cast<ABossMonster_Anubis>(OwnerActor);
	TargetPlayer = Cast<ABasePlayer>(TargetActor);
}

void UPatternObj_Anubis01::StartPattern()
{
	bool bError = !OwnerCharacter.IsValid() || !TargetPlayer.IsValid();
	if (!OwnerCharacter.IsValid())
		UE_LOG(LogTemp, Error, TEXT("UPatternObj_Anubis01::StartPattern OwnerCharacter is nullptr"));
	if (!TargetPlayer.IsValid())
		UE_LOG(LogTemp, Error, TEXT("UPatternObj_Anubis01::StartPattern TargetPlayer is nullptr"));

	if (bError)
		return;

	OwnerCharacter->StartPatternWidget(PatternSolveCount);
	OwnerCharacter->UseOrientRotationToMovement();
	PatternMoveDirection = FMath::VRand().GetSafeNormal2D();
	if (TargetPlayer.IsValid())
	{
		PreState = TargetPlayer->GetPlayerState();
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
	if(OwnerCharacter.IsValid())
		OwnerCharacter->GetMesh()->AddRelativeRotation(CurrentRotation);
}

void UPatternObj_Anubis01::EndPattern()
{
	if (!OwnerCharacter.IsValid() || !TargetPlayer.IsValid())
		return;
	OwnerCharacter->EndPatternWidget();
	OwnerCharacter->UseControllerDesiredRotation();
	bIsPatternActive = false;
	if (TargetPlayer.IsValid())
	{
		TargetPlayer->ChangePlayerState(PreState);
	}
	if (OwnerCharacter->GetMonsterInteractionComponent())
		OwnerCharacter->GetMonsterInteractionComponent()->EndActiveInteraction();
	if (OwnerCharacter->GetMesh())
	{
		OwnerCharacter->GetMesh()->SetRelativeRotation(FRotator{0,-90,0});
	}
	OwnerCharacter = nullptr;
	TargetPlayer = nullptr;
}

void UPatternObj_Anubis01::UpdateCondition()
{
	if (!bIsPatternActive)
		return;
	--CurrentCount;
	CurTime = 0.f;
	OwnerCharacter->UpdatePatternWidget();
	if (CurrentCount <= 0)
	{
		bIsPatternActive = false;
		if (OwnerCharacter.IsValid() && OwnerCharacter.Get())
		{
			OwnerCharacter->GetMesh()->SetRelativeRotation(FRotator(0.f, -90.0f, 0.0f));
			OwnerCharacter->SetStunned(StunDuration);
			if (OwnerCharacter.IsValid() && OwnerCharacter.Get())
			{
				if (OwnerCharacter->GetMonsterInteractionComponent())
					OwnerCharacter->GetMonsterInteractionComponent()->EndActiveInteraction();
			}
			if (OwnerCharacter.IsValid() && OwnerCharacter.Get())
			{
				if (OwnerCharacter->GetAttackComponent())
					OwnerCharacter->GetAttackComponent()->StopAttack();
			}
		}
	}
}
