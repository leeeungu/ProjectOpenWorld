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
#include "Interaction/Component/PlayerInteractionComponent.h"
#include "Building/Component/BuildingAssistComponentV2.h"
#include "Player/Component/PlayerAnimationComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/Animation/PlayerAnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "Building/Widget/BuildingModeWidget.h"
#include "GenericTeamAgentInterface.h"

DEFINE_LOG_CATEGORY(LogBasePlayer);

ABasePlayer::ABasePlayer() : ABaseCharacter()
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

	InteractionComponent = CreateDefaultSubobject<UPlayerInteractionComponent>(TEXT("InteractionComponent"));

	BuildAssistComponent = CreateDefaultSubobject<UBuildingAssistComponentV2>(TEXT("BuildingAssist"));

	PlayerAnimationComponent = CreateDefaultSubobject<UPlayerAnimationComponent>(TEXT("PlayerAnimationComponent"));
	StatusArray.Init(0, (uint8)EStatusType::EnumMax);
	PlayerMoveFunc = &ABasePlayer::MoveTravel;

	//Script/UMGEditor.WidgetBlueprint'/Game/Building/Widget/WBP_BuildModeWidget.WBP_BuildModeWidget'

	static ConstructorHelpers::FClassFinder<UBuildingModeWidget> WidgetClass(TEXT("/Game/Building/Widget/WBP_BuildModeWidget.WBP_BuildModeWidget_C"));
	if (WidgetClass.Succeeded())
	{
		BuildingWidget = CreateWidget< UBuildingModeWidget>(GetWorld(), WidgetClass.Class);
	}
}

void ABasePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABasePlayer::SetTopDownMode(bool bTopDown)
{
	TopDownMode = bTopDown;
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		UE_LOG(LogTemp, Log, TEXT("SetTopDownMode"));
		PlayerController->SetShowMouseCursor(TopDownMode);
	}
	if (TopDownMode)
	{
		EnableInput(PlayerController);
	}
	UE_LOG(LogTemp, Log, TEXT("TopDownMode : %d"), TopDownMode);
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

void ABasePlayer::UpdateWeight(float InventoryWeight)
{
	if (*GetStatusRef(EStatusType::MaxWeight) > InventoryWeight)
	{
		GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 50.0f;
	}
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

bool ABasePlayer::GetStatus(EStatusType StatusType, float& Result) const
{
	if (StatusArray.IsValidIndex((uint8)StatusType))
	{
		Result = StatusArray[(uint8)StatusType];
		return true;
	}
	return false;
}

float ABasePlayer::GetAttackValue_Implementation() const
{
	float Result = 0.0f;
	GetStatus(EStatusType::Attack, Result);
	return Result;
}

void ABasePlayer::SetAttackValue_Implementation(float NewValue)
{
	SetStatus(EStatusType::Attack, NewValue);
}

void ABasePlayer::RetAttackValue_Implementation()
{
}

bool ABasePlayer::DamagedCharacter_Implementation(const TScriptInterface<IAttackInterface>& Other)
{
	if (!Other || !Other.GetObject())
		return false;
	APawn* pOther = Cast < APawn>(Other.GetObject());
	if (pOther  && FGenericTeamId::GetAttitude(GetController(), pOther->GetController()) == ETeamAttitude::Friendly)
	{
		return false;
	}
	float Damage = IAttackInterface::Execute_GetAttackValue(Other.GetObject());
	float Hp{};
	GetStatus(EStatusType::Hp, Hp);
	if (Hp < Damage)
		Damage = Hp;
	Hp -= Damage;
	SetStatus(EStatusType::Hp, Hp);
	if (OnDamagedDelegate.IsBound())
	{
		OnDamagedDelegate.Broadcast(pOther, Damage);
	}
	UE_LOG(LogTemp, Log, TEXT("HP : %f"), Hp);
	if (Hp <= 0.f)
	{
		Hp = 0.0f;
		/*if (GetMesh())
		{
			GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
			GetMesh()->SetSimulatePhysics(true);
			if (pOther)
			{
				GetMesh()->AddForce((GetActorLocation() - pOther->GetActorLocation()).GetSafeNormal() * 1000.f * GetMesh()->GetMass());
			}
		}
		GetMesh()->bPauseAnims = true;
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FTimerHandle handle{};
		bDead = true;
		GetWorldTimerManager().SetTimer(handle, [this]() {Destroy(); }, 4.0f, false, 4.0f);*/
	}
	return true;
}

bool ABasePlayer::IsDead_Implementation() const
{
	return false;
}

float ABasePlayer::GetArchitectSpeed_Implementation() const
{
	return 1.0f;
}

void ABasePlayer::StartArchitect_Implementation(ABaseBuilding* Building)
{
	GetPlayerAnimationComponent()->StartArchitecture();
}

void ABasePlayer::StopArchitect_Implementation(ABaseBuilding* Building)
{
	GetPlayerAnimationComponent()->ResetAnimationState();
}

void ABasePlayer::EndArchitect_Implementation(ABaseBuilding* Building)
{
	GetPlayerAnimationComponent()->ResetAnimationState();
}

float ABasePlayer::GetResourceSpeed_Implementation() const
{
	return 1.0f;
}

void ABasePlayer::StartResource_Implementation(AResourceActor* ResourceActor)
{
	GetPlayerAnimationComponent()->StartMining();
}

void ABasePlayer::StopResource_Implementation(AResourceActor* ResourceActor)
{
	GetPlayerAnimationComponent()->ResetAnimationState();
}

void ABasePlayer::EndResource_Implementation(AResourceActor* ResourceActor)
{
	GetPlayerAnimationComponent()->ResetAnimationState();
}

void ABasePlayer::OnStartEvent(const FInputActionValue& Value, EInputKeyType KeyType)
{
	TScriptInterface<IPlayerInputInterface>* InputInterface =InputMapping.Find(KeyType);
	if (InputInterface && *InputInterface && InputInterface->GetObject())
		(*InputInterface)->StartEvent(Value, KeyType);
}

void ABasePlayer::OnTriggerEvent(const FInputActionValue& Value, EInputKeyType KeyType)
{
	TScriptInterface<IPlayerInputInterface>* InputInterface = InputMapping.Find(KeyType);
	if (InputInterface && *InputInterface && InputInterface->GetObject())
		(*InputInterface)->TriggerEvent(Value, KeyType);
}

void ABasePlayer::OnCompleteEvent(const FInputActionValue& Value, EInputKeyType KeyType)
{
	TScriptInterface<IPlayerInputInterface>* InputInterface = InputMapping.Find(KeyType);
	if (InputInterface && *InputInterface && InputInterface->GetObject())
		(*InputInterface)->CompleteEvent(Value, KeyType);
}

void ABasePlayer::StartEvent(const FInputActionValue& Value, EInputKeyType KeyType)
{
	switch (KeyType)
	{
	case EInputKeyType::WASD:
		break;
	case EInputKeyType::SpaceBar:
		if (TopDownMode)
			break;
		if (!PlayerAnimationComponent || !PlayerAnimationComponent->StartClimb())
		{
			Jump();
		}
		break;
	case EInputKeyType::MouseAxis:
		break;
	case EInputKeyType::KeyF:
		if (InteractionComponent)
		{
			InteractionComponent->OnInteractionStart();
		}
		break;
	case EInputKeyType::Esc:
		break;
	case EInputKeyType::KeyC:
		break;
	case EInputKeyType::MouseR:
		if (BuildAssistComponent && !TopDownMode)
		{
			BuildAssistComponent->SpawnBuilding();
		}
		break;
	case EInputKeyType::MouseL:
		if (BuildAssistComponent && !TopDownMode)
		{
			BuildAssistComponent->SpawnBuilding();
			BuildAssistComponent->EndBuilding();
		}
		break;
	case EInputKeyType::MouseWheel:
	
		break;
	case EInputKeyType::KeyB:
		break;
	default:
		break;
	}
}

void ABasePlayer::TriggerEvent(const FInputActionValue& Value, EInputKeyType KeyType)
{
	switch (KeyType)
	{
	case EInputKeyType::WASD:
		if (PlayerMoveFunc && !TopDownMode)
		{
			(this->*PlayerMoveFunc)(Value);
		}
		break;
	case EInputKeyType::SpaceBar:
		break;
	case EInputKeyType::MouseAxis:
		if (!TopDownMode)
		{
			FVector2D LookAxisVector = Value.Get<FVector2D>();
			if (Controller != nullptr)
			{
				// add yaw and pitch input to controller
				AddControllerYawInput(LookAxisVector.X);
				AddControllerPitchInput(LookAxisVector.Y);
			}
		}
		break;
	case EInputKeyType::KeyF:
		if (InteractionComponent)
		{
			InteractionComponent->OnInteractionTriggered();
		}
		break;
	case EInputKeyType::Esc:
		break;
	case EInputKeyType::KeyC:
		if (InteractionComponent)
		{
			InteractionComponent->OnActorCancel();
		}
		break;
	case EInputKeyType::MouseR:
		break;
	case EInputKeyType::MouseL:
		if (TopDownMode)
		{
			// 화면에서 지면으로 위치를 pick 하고 이동 방향 계산
			APlayerController* PC = Cast<APlayerController>(GetController());
			if (PC)
			{
				float MouseX = 0.f, MouseY = 0.f;
				// 마우스 좌표 얻기
				if (PC->GetMousePosition(MouseX, MouseY))
				{
					FVector WorldOrigin, WorldDir;
					if (PC->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldOrigin, WorldDir))
					{
						const FVector TraceStart = WorldOrigin;
						const FVector TraceEnd = WorldOrigin + WorldDir * 100000.0f;

						FHitResult Hit;
						FCollisionQueryParams Params(SCENE_QUERY_STAT(TopDownClick), true);
						Params.AddIgnoredActor(this);

						GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);
						{
							const FVector HitLocation = Hit.ImpactPoint;

							FVector MoveDirection = (HitLocation - GetActorLocation()).GetSafeNormal2D();

							const FRotator Rotation = MoveDirection.Rotation();
							const FRotator YawRotation(0, Rotation.Yaw, 0);

							const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
							const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

							// add movement 
							AddMovementInput(ForwardDirection, 6);

#if ENABLE_DRAW_DEBUG
							// 디버그 시각화 (에디터/개발용)
							//DrawDebugSphere(GetWorld(), HitLocation, 16.f, 12, FColor::Green, false, 1.0f);
							//DrawDebugLine(GetWorld(), TraceStart, HitLocation, FColor::Green, false, 5.0f, 0, 1.0f);
#endif

						}
					}
				}
			}
		}
		break;
	case EInputKeyType::MouseWheel:
	{
		FVector2D LookAxisVector = Value.Get<FVector2D>();
		if (BuildAssistComponent)
		{
			BuildAssistComponent->RotateBuilding(LookAxisVector.X * 5.0f);
		}
	}
		break;
	case EInputKeyType::KeyB:
		if (BuildingWidget && !TopDownMode)
		{
			BuildingWidget->ToggleWidget();
		}
		break;
	default:
		break;
	}
}

void ABasePlayer::CompleteEvent(const FInputActionValue& Value, EInputKeyType KeyType)
{
	switch (KeyType)
	{
	case EInputKeyType::WASD:
		if (PlayerMoveFunc == &ABasePlayer::MoveClimb)
		{
			if (UPlayerAnimInstance* Instance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
			{
				Instance->SetClimbSpeed(0);
			}
		}
		break;
	case EInputKeyType::SpaceBar:
		StopJumping();
		break;
	case EInputKeyType::MouseAxis:
		break;
	case EInputKeyType::KeyF:
		if (InteractionComponent)
		{
			InteractionComponent->OnInteractionCompleted();
		}
		break;
	case EInputKeyType::Esc:
		if (BuildAssistComponent)
		{
			BuildAssistComponent->EndBuilding();
		}
		if (InteractionComponent)
		{
			InteractionComponent->OnInteractionCompleted();
		}
		break;
	case EInputKeyType::KeyC:
		break;
	case EInputKeyType::MouseR:
		break;
	case EInputKeyType::MouseL:
		break;
	case EInputKeyType::MouseWheel:
		break;
	case EInputKeyType::KeyB:
		break;
	default:
		break;
	}
}

void ABasePlayer::SetInputInterface(EInputKeyType KeyType, TScriptInterface<IPlayerInputInterface> InputInterface)
{
	InputMapping.FindOrAdd(KeyType, InputInterface) = InputInterface;
}

void ABasePlayer::ResetDeaflut(EInputKeyType KeyType)
{
	TScriptInterface<IPlayerInputInterface>& InputInterface = InputMapping.FindOrAdd(KeyType);
		InputInterface = this;
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
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		UInputAction* ArrActions[] = 
		{ 
			MoveAction, LookAction, JumpAction, 
			InteractionAction, EscAction, KeyCAction, 
			MouseRAction, MouseLAction, MouseWheelAction,
			BuildModeAction
		};
		EInputKeyType ArrKeyTypes[] = 
		{ 
			EInputKeyType::WASD, EInputKeyType::MouseAxis, EInputKeyType::SpaceBar,
			EInputKeyType::KeyF, EInputKeyType::Esc, EInputKeyType::KeyC,
			EInputKeyType::MouseR, EInputKeyType::MouseL, EInputKeyType::MouseWheel,
			EInputKeyType::KeyB
		};
		int32 ArrCount = 10;
		for (int32 i = 0; i < ArrCount; ++i)
		{
			if (ArrActions[i])
			{
				EnhancedInputComponent->BindAction(ArrActions[i], ETriggerEvent::Started, this, &ABasePlayer::OnStartEvent, ArrKeyTypes[i]);
				EnhancedInputComponent->BindAction(ArrActions[i], ETriggerEvent::Triggered, this, &ABasePlayer::OnTriggerEvent, ArrKeyTypes[i]);
				EnhancedInputComponent->BindAction(ArrActions[i], ETriggerEvent::Completed, this, &ABasePlayer::OnCompleteEvent, ArrKeyTypes[i]);
			}
			ResetDeaflut(ArrKeyTypes[i]);
		}
	}
	else
	{
		UE_LOG(LogBasePlayer, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ABasePlayer::MoveClimb(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (true)
	{
		StartTravel();
		return;
	}
	if (!PlayerAnimationComponent->ClimbLineCheck())
	{
		StartTravel();
		if (UPlayerAnimInstance* Instance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
		{
			Instance->SetClimbSpeed(0);
		}
		return;
	}
	if (MovementVector.Y > 0.9f && PlayerAnimationComponent->IsEmpthUp())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(ClimbMontage);
		StartTravel();
		return;
	}

	if (GetMesh() != nullptr)
	{
		FVector Right = GetActorRightVector();
		FVector Up = GetActorUpVector();
		FVector MoveDir = (Right * MovementVector.X + Up * MovementVector.Y).GetSafeNormal();

		const FHitResult* Hit = PlayerAnimationComponent->GetPelvisHit();
		FVector Avg = PlayerAnimationComponent->GetAVGPosition();
		FVector Normal = PlayerAnimationComponent->GetAVGNormal();
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
			AddActorWorldOffset(Hit->ImpactNormal * GetCapsuleComponent()->GetScaledCapsuleRadius() * GetWorld()->GetDeltaSeconds() * 2.f);
		}
		else if (Dis > 67.0f)
		{
			AddActorWorldOffset(Hit->ImpactNormal * -GetCapsuleComponent()->GetScaledCapsuleRadius() * GetWorld()->GetDeltaSeconds() * 2.0f);
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
