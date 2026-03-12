#include "Player/Component/PlayerMoveComponent.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"

UPlayerMoveComponent::UPlayerMoveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	MoveTriggeredFunc = &UPlayerMoveComponent::MoveDefault;
	//MoveStartedFunc = &UPlayerMoveComponent::MoveDefault;
	//MoveReleasedFunc = &UPlayerMoveComponent::MoveDefault;
}

void UPlayerMoveComponent::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<ACharacter>(GetOwner());
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

void UPlayerMoveComponent::MoveStarted(const FInputActionValue& Value)
{
	if (!CheckFunction(MoveStartedFunc, TEXT("MoveStartedFunc")))
		return;
	if (!IsMoveable())
		return;
	(this->*MoveStartedFunc)(Value);
}

void UPlayerMoveComponent::MoveTriggered(const FInputActionValue& Value)
{
	if (!CheckFunction(MoveTriggeredFunc, TEXT("MoveTriggeredFunc")))
		return;
	if (!IsMoveable())
		return;
	(this->*MoveTriggeredFunc)(Value);
}

void UPlayerMoveComponent::MoveReleased(const FInputActionValue& Value)
{
	if (!CheckFunction(MoveReleasedFunc, TEXT("MoveReleasedFunc")))
		return;
	if (!IsMoveable())
		return;
	(this->*MoveReleasedFunc)(Value);
}

void UPlayerMoveComponent::MoveDefault(const FInputActionValue& Value)
{
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
