#include "GameBase/Animation/BaseAnimInstance.h"
#include "GameBase/MetaData/AMDLoop.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

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
		if (UAnimMontage* NextMontage = MontageQueue->GetMontage())
		{
			Montage_Play(NextMontage);
			if (CurrentMontage != NextMontage)
			{
				if (UAMDLoop* LoopData = NextMontage->FindMetaDataByClass<UAMDLoop>())
				{
					LoopObject = LoopData->CreateInstanceObject(GetWorld());
					if (LoopObject)
						LoopObject->Initialize(this, LoopData);
				}
			}
			CurrentMontage = NextMontage;
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