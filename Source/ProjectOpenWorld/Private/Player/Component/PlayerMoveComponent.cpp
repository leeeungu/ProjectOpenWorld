#include "Player/Component/PlayerMoveComponent.h"
#include "InputActionValue.h"
#include "Player/Character/BasePlayer.h"

UPlayerMoveComponent::UPlayerMoveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bIsMoveable = true;
	SetDefaultMove();
}

void UPlayerMoveComponent::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<ABasePlayer>(GetOwner());
	bIsMoveable = true;
	if(Player.IsValid())
		Controller = Cast<APlayerController>(Player->GetController());
}

bool UPlayerMoveComponent::CheckFunction(void(UPlayerMoveComponent::* Func)(const FInputActionValue&), const FString& FunctionName) const
{
	if(!Func)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerMoveComponent: Function pointer is null. FunctionName: %s"), *FunctionName);	
		return false;
	}
	if(!Player.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerMoveComponent: Player reference is invalid."));
		return false;
	}
	if (!Controller.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerMoveComponent: Controller reference is invalid."));
		return false;
	}
	return true;
}

void UPlayerMoveComponent::StartEvent(const FInputActionValue& Value, EInputKeyType KeyType)
{
	CurrentKeyType = KeyType;
	if (!CheckFunction(MoveStartedFunc, TEXT("MoveStartedFunc")))
		return;
	if (!IsMoveable())
		return;
	(this->*MoveStartedFunc)(Value);
}

void UPlayerMoveComponent::TriggerEvent(const FInputActionValue& Value, EInputKeyType KeyType)
{
	CurrentKeyType = KeyType;
	if (!CheckFunction(MoveTriggeredFunc, TEXT("MoveTriggeredFunc")))
		return;
	if (!IsMoveable())
		return;
	(this->*MoveTriggeredFunc)(Value);
}

void UPlayerMoveComponent::CompleteEvent(const FInputActionValue& Value, EInputKeyType KeyType)
{
	CurrentKeyType = KeyType;
	if (!CheckFunction(MoveReleasedFunc, TEXT("MoveReleasedFunc")))
		return;
	if (!IsMoveable())
		return;
	(this->*MoveReleasedFunc)(Value);
}

void UPlayerMoveComponent::SetTopDownMode()
{
	MoveTriggeredFunc = &UPlayerMoveComponent::TriggerTopDownMode;

	if (Player.IsValid())
	{
		Player->SetInputInterface(EInputKeyType::WASD, nullptr);
		Player->SetInputInterface(EInputKeyType::MouseL, this);
	}
}

void UPlayerMoveComponent::SetDefaultMove()
{
	MoveTriggeredFunc = &UPlayerMoveComponent::TriggerDefault;
	if (Player.IsValid())
	{
		Player->SetInputInterface(EInputKeyType::WASD, this);
		Player->SetInputInterface(EInputKeyType::MouseL, Player.Get());
	}
	//MoveStartedFunc = &UPlayerMoveComponent::MoveDefault;
	//MoveReleasedFunc = &UPlayerMoveComponent::MoveDefault;
}

void UPlayerMoveComponent::SetSwordMove()
{
	MoveTriggeredFunc = &UPlayerMoveComponent::TriggerSword;
	if (Player.IsValid())
	{
		Player->SetInputInterface(EInputKeyType::WASD, this);
		Player->SetInputInterface(EInputKeyType::MouseL, Player.Get());
	}
}

void UPlayerMoveComponent::TriggerDefault(const FInputActionValue& Value)
{
	if (Player->GetPlayerState() != EPlayerState::Travel && CurrentKeyType != EInputKeyType::WASD)
		return;
	FVector2D MovementVector = Value.Get<FVector2D>();

	//if (PlayerAnimationComponent->IsClimbing())
	//{
	//	StartClimb();
	//	return;
	//}
	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement 
	Player->AddMovementInput(ForwardDirection, MovementVector.Y);
	Player->AddMovementInput(RightDirection, MovementVector.X);
}

void UPlayerMoveComponent::StartDefault(const FInputActionValue& Value)
{
}

void UPlayerMoveComponent::CompleteDefault(const FInputActionValue& Value)
{
}

void UPlayerMoveComponent::TriggerSword(const FInputActionValue& Value)
{
	TriggerDefault(Value);
}

void UPlayerMoveComponent::TriggerTopDownMode(const FInputActionValue& Value)
{
	if (Player->GetPlayerState() != EPlayerState::TopDown && CurrentKeyType != EInputKeyType::MouseL)
		return;

	// 화면에서 지면으로 위치를 pick 하고 이동 방향 계산
		float MouseX = 0.f, MouseY = 0.f;
		// 마우스 좌표 얻기
		if (Controller->GetMousePosition(MouseX, MouseY))
		{
			FVector WorldOrigin, WorldDir;
			if (Controller->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldOrigin, WorldDir))
			{
				const FVector TraceStart = WorldOrigin;
				const FVector TraceEnd = WorldOrigin + WorldDir * 100000.0f;

				FHitResult Hit;
				FCollisionQueryParams Params(SCENE_QUERY_STAT(TopDownClick), true);
				Params.AddIgnoredActor(Player.Get());

				GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);
				{
					const FVector HitLocation = Hit.ImpactPoint;

					FVector MoveDirection = (HitLocation - Player->GetActorLocation()).GetSafeNormal2D();

					const FRotator Rotation = MoveDirection.Rotation();
					const FRotator YawRotation(0, Rotation.Yaw, 0);

					const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
					const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

					// add movement 
					Player->AddMovementInput(ForwardDirection, 6);

#if ENABLE_DRAW_DEBUG
					// 디버그 시각화 (에디터/개발용)
					//DrawDebugSphere(GetWorld(), HitLocation, 16.f, 12, FColor::Green, false, 1.0f);
					//DrawDebugLine(GetWorld(), TraceStart, HitLocation, FColor::Green, false, 5.0f, 0, 1.0f);
#endif

				}
			}
	}
}
