#include "GameBase/Animation/BaseAnimInstance.h"
#include "GameBase/MetaData/AMDLoop.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameBase/MetaData/AMD_MontageChangeEvent.h"

void UBaseAnimInstance::OnMontageStartedFunc(UAnimMontage* Montage)
{
	if (MontageQueue && MontageQueue.GetObject())
	{
		MontageQueue->MontageStartEvent(this, Montage);
	}
}

void UBaseAnimInstance::OnMontageBlendingOutFunc(UAnimMontage* Montage, bool bInterrupted)
{
	if (MontageQueue && MontageQueue.GetObject())
	{
		//Montage->GetAssetUserDataOfClass<>();
		if (LoopObject)
		{
			LoopObject->UpdateBlendOut();
			if (!IsLoop())
			{
				LoopObject->EndLoop();
				LoopObject = nullptr;
			}
		}
		MontageQueue->MontageBlendingEvent(this, Montage, bInterrupted);
	}
}

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OnMontageStarted.AddDynamic(this, &UBaseAnimInstance::OnMontageStartedFunc);
	OnMontageBlendingOut.AddDynamic(this, &UBaseAnimInstance::OnMontageBlendingOutFunc);
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

void UBaseAnimInstance::SetMontageQueueInterface(TScriptInterface<IMontageQueueInterface> Interface)
{
	MontageQueue = Interface;
}

void UBaseAnimInstance::PlayMontageQueue()
{
	if (MontageQueue && MontageQueue.GetObject())
	{
		UAnimMontage* NextMontage = MontageQueue->GetMontage();
		{
			Montage_Play(NextMontage);
			if (CurrentMontage != NextMontage)
			{
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
				if (NextMontage)
				{
					const TArray<UAnimMetaData*>& MetaData = NextMontage->GetMetaData();
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
				CurrentMontage = NextMontage;
			}
		}
	}
}

bool UBaseAnimInstance::MontageIsPlay(UAnimMontage* Montage)
{
	return CurrentMontage == Montage;
}

bool UBaseAnimInstance::IsLoop() const 
{
	if (!LoopObject)
		return false;
	return LoopObject->IsLoop();
}