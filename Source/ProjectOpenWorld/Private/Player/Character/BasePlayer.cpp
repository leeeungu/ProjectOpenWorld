#include "Player/Character/BasePlayer.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Interaction/Component/InteractionComponent.h"
#include "Building/BuildingAssistComponent.h"
#include "Player/Component/PlayerAnimationComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/Animation/PlayerAnimInstance.h"

DEFINE_LOG_CATEGORY(LogBasePlayer);

ABasePlayer::ABasePlayer()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

	BuildAssistComponent = CreateDefaultSubobject<UBuildingAssistComponent>(TEXT("BuildingAssist"));

	PlayerAnimationComponent = CreateDefaultSubobject<UPlayerAnimationComponent>(TEXT("PlayerAnimationComponent"));
	StatusArray.Init(0, (uint8)EStatusType::EnumMax);
	PlayerMoveFunc = &ABasePlayer::MoveTravel;
}

void ABasePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABasePlayer::StartClimb()
{
	if (PlayerAnimationComponent->StartClimb())
		PlayerMoveFunc = &ABasePlayer::MoveClimb;
}

void ABasePlayer::StartTravel()
{
	if(PlayerAnimationComponent->StartTravel())
		PlayerMoveFunc = &ABasePlayer::MoveTravel;
}

void ABasePlayer::BeginPlay()
{
	Super::BeginPlay();
}

void ABasePlayer::SetStatus(EStatusType StatusType, float Value)
{
	if (StatusArray.IsValidIndex((uint8)StatusType))
	{
		StatusArray[(uint8)StatusType] = Value;
	}
}

bool ABasePlayer::GetStatus(EStatusType StatusType, float& Result)
{
	if (StatusArray.IsValidIndex((uint8)StatusType))
	{
		Result = StatusArray[(uint8)StatusType];
		return true;
	}
	return false;
}

void ABasePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABasePlayer::OnJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABasePlayer::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ABasePlayer::OnMoveCompleted);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABasePlayer::Look);

		//Interaction
		EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Started, this, &ABasePlayer::OnInteractionStart);
		EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Triggered, this, &ABasePlayer::OnInteraction);
		EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Completed, this, &ABasePlayer::OnInteractionEnd);

		EnhancedInputComponent->BindAction(EscAction, ETriggerEvent::Completed, this, &ABasePlayer::OnActionExit);
		EnhancedInputComponent->BindAction(KeyCAction, ETriggerEvent::Triggered, this, &ABasePlayer::OnActionKeyC);

		EnhancedInputComponent->BindAction(MouseRAction, ETriggerEvent::Started, this, &ABasePlayer::OnActionMouseR);
		EnhancedInputComponent->BindAction(MouseLAction, ETriggerEvent::Started, this, &ABasePlayer::OnActionMouseL);
		EnhancedInputComponent->BindAction(MouseWheelAction, ETriggerEvent::Triggered, this, &ABasePlayer::OnActionMouseWheel);
	}
	else
	{
		UE_LOG(LogBasePlayer, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ABasePlayer::Move(const FInputActionValue& Value)
{
	if (PlayerMoveFunc)
	{
		(this->*PlayerMoveFunc)(Value);
	}
}
void ABasePlayer::OnMoveCompleted(const FInputActionValue& Value)
{
	if (PlayerMoveFunc == &ABasePlayer::MoveClimb)
	{
		if (UPlayerAnimInstance* Instance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
		{
			Instance->SetClimbSpeed(0);
		}
	}
}

void ABasePlayer::MoveClimb(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (!PlayerAnimationComponent->ClimbLineCheck())
	{
		StartTravel();
		if (UPlayerAnimInstance* Instance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
		{
			Instance->SetClimbSpeed(0);
		}
		return;
	}
	;
	if (GetMesh() != nullptr)
	{
		FVector Right = GetActorRightVector();
		FVector Up = GetActorUpVector();
		FVector MoveDir = (Right * MovementVector.X + Up * MovementVector.Y).GetSafeNormal();

		const FHitResult* Hit = PlayerAnimationComponent->GetPelvisHit();
		FVector Avg = PlayerAnimationComponent->GetAVGPosition();
		FVector Normal = PlayerAnimationComponent->GetCenterNoraml();
		if (UPlayerAnimInstance* Instance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
		{
			double DotAngle = FMath::RadiansToDegrees(FMath::Acos(FVector2D::DotProduct(FVector2D(0, 1), MovementVector.GetSafeNormal())));
			double Angle = FVector2D::DotProduct(FVector2D(1, 0), MovementVector.GetSafeNormal()) > 0 ? DotAngle : -DotAngle;
			Instance->SetClimbDirection(Angle);
			Instance->SetClimbSpeed(MovementVector.GetSafeNormal().Size());
		}
		float Dis = FVector::Distance(GetActorLocation(), Avg);
		if (Dis < 65.0f)
		{
			AddActorWorldOffset(Hit->Normal * GetCapsuleComponent()->GetScaledCapsuleRadius() * GetWorld()->GetDeltaSeconds() * 2.f);
		}
		else if (Dis > 67.0f)
		{
			AddActorWorldOffset(Hit->Normal * -GetCapsuleComponent()->GetScaledCapsuleRadius() * GetWorld()->GetDeltaSeconds() * 2.f);
		}
		FRotator Rotation = GetActorRotation();
		Rotation.Roll = 0.0f;
		FRotator NewRotation = (Normal * -1).Rotation();
		NewRotation.Roll = 0.0f;
		SetActorRotation(FMath::RInterpTo(Rotation, NewRotation, GetWorld()->GetDeltaSeconds(), 20.0f));
		AddActorWorldOffset(MoveDir* 200.0f * GetWorld()->GetDeltaSeconds(), false);
	}

}

void ABasePlayer::MoveTravel(const FInputActionValue& Value)
{	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (PlayerAnimationComponent->IsClimbing())
	{
		StartClimb();
		return;
	}
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABasePlayer::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABasePlayer::OnInteractionStart(const FInputActionValue& Value)
{
	if (InteractionComponent)
	{
		InteractionComponent->OnInteractionStart();
	}
}

void ABasePlayer::OnInteraction(const FInputActionValue& Value)
{
	if (InteractionComponent)
	{
		InteractionComponent->OnInteractionTriggered();
	}
}

void ABasePlayer::OnInteractionEnd(const FInputActionValue& Value)
{
	if (InteractionComponent)
	{
		InteractionComponent->OnInteractionCompleted();
	}
}

void ABasePlayer::OnActionExit(const FInputActionValue& Value)
{
	if (BuildAssistComponent)
	{
		BuildAssistComponent->EndBuilding();
	}
}

void ABasePlayer::OnActionKeyC(const FInputActionValue& Value)
{
	if (InteractionComponent)
	{
		InteractionComponent->OnActorCancel();
	}
}

void ABasePlayer::OnActionMouseWheel(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (BuildAssistComponent)
	{
		BuildAssistComponent->RotateBuilding(LookAxisVector.X * 5.0f);
	}
}

void ABasePlayer::OnActionMouseR(const FInputActionValue& Value)
{
	if (BuildAssistComponent)
	{
		BuildAssistComponent->SpawnBuilding();
	}
}

void ABasePlayer::OnActionMouseL(const FInputActionValue& Value)
{
	if (BuildAssistComponent)
	{
		BuildAssistComponent->SpawnBuilding();
		BuildAssistComponent->EndBuilding();
	}
}


void ABasePlayer::OnJump()
{
	if (PlayerAnimationComponent && PlayerAnimationComponent->CanClimb())
	{
		PlayerAnimationComponent->StartClimb();
	}
	else
	{
		Jump();
	}
}