#include "GameBase/Animation/BaseAnimInstance.h"
#include "GameBase/MetaData/AMDLoop.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameBase/MetaData/AMD_MontageChangeEvent.h"

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OnMontageStarted.AddUniqueDynamic(this, &UBaseAnimInstance::OnMontageStartedEvent);
	OnMontageBlendingOut.AddUniqueDynamic(this, &UBaseAnimInstance::OnMontageBlendingOutEvent);
	if (ACharacter* Owner = Cast<ACharacter>(TryGetPawnOwner()))
	{
		MovementComponent = Owner->GetCharacterMovement();
	}
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!MovementComponent)
		return;
	IsFalling = MovementComponent->IsFalling();
	MoveSpeed = 0.0f;
	if (MovementComponent->GetMaxSpeed() > 0)
		MoveSpeed = MovementComponent->Velocity.Length() / MovementComponent->GetMaxSpeed();
	MoveSpeed = FMath::Clamp(MoveSpeed, 0.0f, 1.0f);
	if (LoopObject)
	{
		LoopObject->UpdateLoop(DeltaSeconds);
		if (!IsLoop())
		{
			LoopObject->EndLoop();
			LoopObject = nullptr;
		}
	}
}

void UBaseAnimInstance::OnMontageStartedEvent(UAnimMontage* Montage)
{
}


void UBaseAnimInstance::OnMontageBlendingOutEvent(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bIsPlayingMontage)
		return;
	bool bWasLooping = false;
	if (LoopObject)
	{
		LoopObject->UpdateBlendOut();
		bWasLooping = IsLoop();
		if (!bWasLooping)
		{
			LoopObject->EndLoop();
			LoopObject = nullptr;
		}
	}
	if (!bWasLooping)
	{
		CurrentMontageIndex++;
	}
	if (CanPlayMontage())
	{
		if (CurrentMontage && !bWasLooping)
		{
			const TArray<UAnimMetaData*>& MetaData = CurrentMontage->GetMetaData();
			for (UAnimMetaData* Data : MetaData)
			{
				if (UAMD_MontageChangeEvent* ChangeEvent = Cast<UAMD_MontageChangeEvent>(Data))
				{
					ChangeEvent->EndEvent(this);
				}
			}
		}
		PlayMontage();
		return;
	}
	OnMontageEnd();
}

void UBaseAnimInstance::ChangeMontageArray(const TArray<UAnimMontage*>& NewMontageArray)
{
	if (bIsPlayingMontage)
		return;
	MontageArray = NewMontageArray;
}

bool UBaseAnimInstance::PlayMontageQueue()
{
	if (MontageArray.IsEmpty() || bIsPlayingMontage)
		return false;
	bIsPlayingMontage = true;
	CurrentMontageIndex = 0;
	CurrentMontage = nullptr;
	LoopObject = nullptr;
	PlayMontage();
	return true;
}

void UBaseAnimInstance::StopMontageQueue()
{
	OnMontageEnd();
	Montage_Stop(0.01f);
}

bool UBaseAnimInstance::CanPlayMontage()
{
	return MontageArray.IsValidIndex(CurrentMontageIndex);
}

void UBaseAnimInstance::OnMontageEnd()
{
	//UE_LOG(LogTemp, Warning, TEXT("%s UCharacterMontageComponent :: OnMontageEnd "), *GetName());
	if (CurrentMontage)
	{
		const TArray<UAnimMetaData*>& MetaData = CurrentMontage->GetMetaData();
		for (UAnimMetaData* Data : MetaData)
		{
			if (UAMD_MontageChangeEvent* ChangeEvent = Cast<UAMD_MontageChangeEvent>(Data))
			{
				ChangeEvent->EndEvent(this);
			}
		}
	}
	bIsPlayingMontage = false;
	CurrentMontageIndex = 0;
	CurrentMontage = nullptr;
	LoopObject = nullptr;
	if (OnMontageQueueEnd.IsBound())
	{
		OnMontageQueueEnd.Broadcast();
	}
	UE_LOG(LogTemp, Warning, TEXT("%s UCharacterMontageComponent :: OnMontageEnd Montage Queue End "), *GetName());
}

void UBaseAnimInstance::PlayMontage()
{
	UAnimMontage* NextMontage{};
	if (MontageArray.IsValidIndex(CurrentMontageIndex))
		NextMontage = MontageArray[CurrentMontageIndex];
	if (NextMontage)
	{
		Montage_Play(NextMontage);
		if (CurrentMontage != NextMontage)
		{
			CurrentMontage = NextMontage;
			const TArray<UAnimMetaData*>& MetaData = CurrentMontage->GetMetaData();
			LoopObject = nullptr;
			for (UAnimMetaData* Data : MetaData)
			{
				if (UAMD_MontageChangeEvent* ChangeEvent = Cast<UAMD_MontageChangeEvent>(Data))
				{
					ChangeEvent->StartEvent(this);
				}
				if (UAMDLoop* LoopData = Cast<UAMDLoop>(Data))
				{
					LoopObject = LoopData->CreateInstanceObject(GetWorld());
					if (LoopObject)
						LoopObject->Initialize(this, LoopData);
				}
			}
		}
	}
	else
	{
		OnMontageEnd();
	}
}

bool UBaseAnimInstance::IsLoop() const
{
	if (!LoopObject)
		return false;
	return LoopObject->IsLoop();
}