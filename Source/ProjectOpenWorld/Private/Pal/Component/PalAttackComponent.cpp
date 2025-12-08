#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Controller/PalAIController.h"
#include "Navigation/PathFollowingComponent.h"

UPalAttackComponent::UPalAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	AttackDistance = 50.0f;
}

void UPalAttackComponent::BeginPlay()
{
	Super::BeginPlay();
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
		Controller = Cast< APalAIController>(Pawn->GetController());
	if (Controller)
	{
		Controller->ReceiveMoveCompleted.AddDynamic(this, &UPalAttackComponent::FinishMove);
	}
}

void UPalAttackComponent::TargetIsDead(AActor* Actor)
{
	EndAttack();
}

void UPalAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bCanAttack && AttackData.TargetActor)
	{
		bAttacking = FVector::Distance(AttackData.TargetActor->GetActorLocation(), GetOwner()->GetActorLocation()) < AttackDistance;
		if (bMoveStarted == false && Controller && bAttacking == false)
		{
			bMoveStarted = true;
			bAttacking = false;
			Controller->MoveToActor(AttackData.TargetActor, AttackDistance);
		}
	}
}

void UPalAttackComponent::SetAttackData(FPalAttackData sData)
{
	if (!sData.TargetActor)
		return;
	sData.TargetActor->OnDestroyed.AddDynamic(this, &UPalAttackComponent::TargetIsDead);
	bCanAttack = true;
	AttackData = sData;
	bAttacking = false;
	Current = Default;
	if(AttackData.AttackSlot == 1)
		Current = Skill01;
}

void UPalAttackComponent::StartAttack()
{
	if (!bCanAttack)
	{
		EndAttack();
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("UPalAttackComponent :: StartAttack"));
	Controller->MoveToActor(AttackData.TargetActor, AttackDistance);
	if (OnPalAttackStart.IsBound())
	{
		OnPalAttackStart.Broadcast();
	}
}

void  UPalAttackComponent::EndAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("UPalAttackComponent :: EndAttack"));
	bCanAttack = false;
	if (AttackData.TargetActor)
	{
		AttackData.TargetActor->OnDestroyed.RemoveDynamic(this, &UPalAttackComponent::TargetIsDead);
	}
	if (OnPalAttackEnd.IsBound())
	{
		OnPalAttackEnd.Broadcast();
	}
	AttackData.TargetActor = nullptr;
}

void UPalAttackComponent::FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (bCanAttack == false || bMoveStarted == false || Result != EPathFollowingResult::Type::Success)
		return;
	bMoveStarted = false;
	bAttacking = true;
}