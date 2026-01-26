#include "Player/Component/PlayerAttackComponent.h"
#include "Player/Character/BasePlayer.h"
#include "GameBase/Animation/BaseAnimInstance.h"

UPlayerAttackComponent::UPlayerAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerAttackComponent::BeginPlay()
{
	Super::BeginPlay();
	if (PlayerAttackDataTable)
	{
		TArray< FPlayerAttackData*> TempArray;
		PlayerAttackDataTable->GetAllRows("", TempArray);
		PlayerAttackDataArray.Init(nullptr, static_cast<uint8>(EPlayerAttackType::EnumMax));
		for (FPlayerAttackData* Data : TempArray)
		{
			PlayerAttackDataArray[(uint8)Data->PlayerAttackType] = Data;
		}
		MontageCoolTime.Init(0.0f, static_cast<uint8>(EPlayerAttackType::EnumMax));
		/*PlayerAttackDataArray.Sort(
			[](const FPlayerAttackData& A, const FPlayerAttackData& B)
			{
			return static_cast<uint8>(A.PlayerAttackType) < static_cast<uint8>(B.PlayerAttackType);
			}
		);*/
	}
	ABasePlayer* Player = Cast<ABasePlayer>(GetOwner());
	if (Player && Player->GetMesh())
	{
		
		AnimInstance = Cast< UBaseAnimInstance>(Player->GetMesh()->GetAnimInstance());
		if (AnimInstance)
		{
			AnimInstance->OnMontageQueueEnd.AddUniqueDynamic(this, &UPlayerAttackComponent::OnAttackMontageEnd);
		}
	}
}

void UPlayerAttackComponent::OnAttackMontageEnd()
{
	CurrentAttackType =  EPlayerAttackType::None;
	bIsAttacking = false;
	if (OnPlayerAttackEnd.IsBound())
	{
		OnPlayerAttackEnd.Broadcast();
	}
}

void UPlayerAttackComponent::Attack(EPlayerAttackType InAttackType)
{
	uint8 AttackIndex = static_cast<uint8>(InAttackType);
	UE_LOG(LogTemp, Warning, TEXT("UPlayerAttackComponent::Attack %d"), AttackIndex);
	if (bIsAttacking || !PlayerAttackDataArray.IsValidIndex(AttackIndex) || !PlayerAttackDataArray[AttackIndex])
		return;
	if (MontageCoolTime[AttackIndex] > 0)
		return;
	AnimInstance->ChangeMontageArray(PlayerAttackDataArray[AttackIndex]->AttackMontageArray);
	MontageCoolTime[AttackIndex] = PlayerAttackDataArray[AttackIndex]->CoolTime;
	if(AnimInstance->PlayMontageQueue())
	{
		bIsAttacking = true;
		CurrentAttackType = InAttackType;
		if (OnPlayMontage.IsBound())
		{
			OnPlayMontage.Broadcast(CurrentAttackType);
		}
	}
}

void UPlayerAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	for (int i = 0; i < MontageCoolTime.Num(); ++i)
	{
		if (MontageCoolTime[i] > 0.0f)
		{
			MontageCoolTime[i] -= DeltaTime;
			if (MontageCoolTime[i] <= 0.0f)
				MontageCoolTime[i] = 0.0f;
		}
	}
}

void UPlayerAttackComponent::StopAttack()
{
	AnimInstance->StopMontageQueue();
	OnAttackMontageEnd();
}

float UPlayerAttackComponent::GetMontageCoolTime(EPlayerAttackType InAttackType) const
{
	uint8 AttackIndex = static_cast<uint8>(InAttackType);
	if (PlayerAttackDataArray.IsValidIndex(AttackIndex) && PlayerAttackDataArray[AttackIndex])
	{
		return PlayerAttackDataArray[AttackIndex]->CoolTime;
	}
	return 0.0f;
}

