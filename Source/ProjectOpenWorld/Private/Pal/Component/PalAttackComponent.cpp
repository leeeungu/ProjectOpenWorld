#include "Pal/Component/PalAttackComponent.h"

UPalAttackComponent::UPalAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPalAttackComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPalAttackComponent::TargetIsDead(AActor* Actor, EEndPlayReason::Type EndPlayReason)
{
	EndAttack();
}

void UPalAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPalAttackComponent::SetAttackData(FPalAttackData sData)
{
	if (!sData.TargetActor)
		return;
	sData.TargetActor->OnEndPlay.AddDynamic(this, &UPalAttackComponent::TargetIsDead);
	bCanAttack = true;
	AttackData = sData;
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
		AttackData.TargetActor->OnEndPlay.RemoveDynamic(this, &UPalAttackComponent::TargetIsDead);
	}
	if (OnPalAttackEnd.IsBound())
	{
		OnPalAttackEnd.Broadcast();
	}
	AttackData.TargetActor = nullptr;
}

