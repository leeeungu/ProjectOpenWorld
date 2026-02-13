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
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/Animation/PlayerAnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "GenericTeamAgentInterface.h"
#include "NavigationInvokerComponent.h"
#include "Player/Component/PlayerAttackComponent.h"
#include "Player/Component/PlayerDetectCollision.h"
#include "Player/Component/PlayerItemComponent.h"
#include "GameBase/Component/StatComponent.h"
#include "GameBase/Component/StatComponent_Level.h"
#include "Player/Controller/BasePlayerController.h"
#include "Player/Interface/MainWidgetInterface.h"
#include "GameBase/Widget/PlayerGameOver.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundCue.h"
#include "Building/BaseBuilding.h"

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

	NavigationInvokerComp = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavigationInvokerComp"));

	PlayerAttackComponent = CreateDefaultSubobject<UPlayerAttackComponent>(TEXT("PlayerAttackComponent"));
	///Script/Engine.DataTable'/Game/Player/Input/DataTable/DT_PlayerInput.DT_PlayerInput'
	ConstructorHelpers::FObjectFinder<UDataTable> InputDataTableObj(TEXT("/Game/Player/Input/DataTable/DT_PlayerInput.DT_PlayerInput"));
	if (InputDataTableObj.Succeeded())
	{
		InputDataTable = InputDataTableObj.Object;
	}
	///Script/UMGEditor.WidgetBlueprint'/Game/Widget/WBP_GameOver.WBP_GameOver'
	ConstructorHelpers::FClassFinder<UUserWidget> OverWidgetClass(TEXT("/Game/Widget/WBP_GameOver.WBP_GameOver_C"));
	if (OverWidgetClass.Succeeded())
	{
		GameOverWidgetClass = OverWidgetClass.Class;

	}

	PlayerDetectCollision = CreateDefaultSubobject<UPlayerDetectCollision>(TEXT("PlayerDetectCollision"));
	PlayerDetectCollision->SetupAttachment(RootComponent);

	PlayerItemManagerComponent = CreateDefaultSubobject<UPlayerItemComponent>(TEXT("PlayerItemManagerComponent"));
	StatComponent_Level = CreateDefaultSubobject<UStatComponent_Level>(TEXT("StatComponent_Level"));

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMeshComponent"));
	WeaponMeshComponent->SetupAttachment(GetMesh(), TEXT("WeaponR_Sword"));
}

void ABasePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABasePlayer::SetTopDownMode(bool bTopDown)
{
	if (bTopDown)
	{
		RemoveFromViewPort(MainWidgetInterface);
		ChangePlayerState(EPlayerState::TopDown);
	}
	else
	{
		ChangePlayerState(EPlayerState::Battle);
	}
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
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		GameOverWidget = CreateWidget<UPlayerGameOver>(PlayerController, GameOverWidgetClass);
	}

	//SetStatus(EStatusType::Hp, *GetStatusRef(EStatusType::MaxHp));
	HPStat->SetCurrentStat(*GetStatusRef(EStatusType::MaxHp));
	HPStat->SetMaxStat(*GetStatusRef(EStatusType::MaxHp));
	StatComponent_Level->OnLevelUp.AddUniqueDynamic(this, &ABasePlayer::OnLevelUpEvent);
}

void ABasePlayer::OnLevelUpEvent(int32 OldLevel, bool IsMaxLevel)
{
	if (IsMaxLevel)
		return;
	float MaxHp = *GetStatusRef(EStatusType::MaxHp);
	float NewMaxHp = MaxHp + 50;
	SetStatus(EStatusType::MaxHp, NewMaxHp);
	HPStat->SetMaxStat(NewMaxHp);
	HPStat->SetCurrentStat(NewMaxHp);
	AttackStat->AddCurrentStat(20.0f);
}

void ABasePlayer::SetWeaponMesh(USkeletalMesh* NewMesh, FName SocketName)
{
	if (WeaponMeshComponent)
	{
		WeaponMeshComponent->SetSkeletalMesh(NewMesh);
		WeaponMeshComponent->SetRelativeTransform(FTransform::Identity);
		WeaponMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketName);
	}
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
	switch (StatusType)
	{
	case EStatusType::None:
		break;
	case EStatusType::Hp:
		Result = HPStat->GetCurrentStat();
		break;
	case EStatusType::MaxHp:
		Result = HPStat->GetMaxStat();
		break;
	case EStatusType::Attack:
		Result = AttackStat->GetCurrentStat();
		break;
	case EStatusType::Defense:
		Result = DefendStat->GetCurrentStat();
		break;
	case EStatusType::Shield:
	case EStatusType::MaxShield:
	case EStatusType::Health:
	case EStatusType::MaxHealth:
	case EStatusType::Stamina:
	case EStatusType::WorkSpeed:
	case EStatusType::MaxWeight:
	case EStatusType::EnumMax:
	default:
		if (StatusArray.IsValidIndex((uint8)StatusType))
		{
			Result = StatusArray[(uint8)StatusType];
			return true;
		}
		break;
	}
	
	return false;
}

void ABasePlayer::ChangePlayerState(EPlayerState NewState)
{
	if (NewState == CurrentPlayerState)
		return;
	EPlayerState PrevState = CurrentPlayerState;
	switch (PrevState)
	{
	case EPlayerState::Travel:
	{
		break;
	}
	case EPlayerState::Climb:
		break;
	case EPlayerState::Battle:
		break;
	case EPlayerState::TopDown:
	{
		TopDownMode = false;
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (PlayerController)
		{
			PlayerController->SetShowMouseCursor(false);
		}
		UPlayerInteractionComponent* InteractionComp = Cast<UPlayerInteractionComponent>(InteractionComponent);
		if (InteractionComp)
		{
			InteractionComp->SetCanAiming(true);
		}
		break;
	}
	case EPlayerState::EnumMax:
		break;
	default:
		break;
	}
	CurrentPlayerState = NewState;
	if (OnStateChangeDelegate.IsBound())
	{
		OnStateChangeDelegate.Broadcast(PrevState);
	}
	switch (CurrentPlayerState)
	{
	case EPlayerState::Travel:
	{
		UE_LOG(LogBasePlayer, Log, TEXT("Travel Mode"));
		if (PlayerAnimationComponent->StartTravel())
			PlayerMoveFunc = &ABasePlayer::MoveTravel;
		UseOrientRotationToMovement();
		break;
	}
	case EPlayerState::Climb:
	{
		if (PlayerAnimationComponent->StartClimb())
			PlayerMoveFunc = &ABasePlayer::MoveClimb;
		break;
	}
	case EPlayerState::Battle:
		//UE_LOG(LogBasePlayer, Log, TEXT("Battle Mode"));
		UseControllerDesiredRotation();
		break;
	case EPlayerState::TopDown:
	{
		TopDownMode = true;
		UseOrientRotationToMovement();
		UPlayerInteractionComponent* InteractionComp = Cast<UPlayerInteractionComponent>(InteractionComponent);
		if (InteractionComp)
		{
			InteractionComp->SetCanAiming(false);
		}
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (PlayerController)
		{
			//UE_LOG(LogTemp, Log, TEXT("SetTopDownMode"));
			PlayerController->SetShowMouseCursor(true);

		}
		//UE_LOG(LogBasePlayer, Log, TEXT("TopDown Mode"));
		//EnableInput(PlayerController);
		BuildAssistComponent->EndBuilding();
		break;
	}
	case EPlayerState::EnumMax:
		break;
	default:
		break;
	}
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
	if (!Other || !Other.GetObject() || bDead)
		return false;
	APawn* pOther = Cast < APawn>(Other.GetObject());
	if (pOther  && FGenericTeamId::GetAttitude(GetController(), pOther->GetController()) == ETeamAttitude::Friendly)
	{
		UE_LOG(LogTemp, Log, TEXT("Friendly Fire Disabled"));
		return false;
	}
	float Damage = IAttackInterface::Execute_GetAttackValue(Other.GetObject());
	//GetStatus(EStatusType::Hp, Hp);
	Damage = HPStat->AddCurrentStat(-Damage);
	if (OnDamagedDelegate.IsBound())
	{
		OnDamagedDelegate.Broadcast(pOther, Damage);
	}
	if (HPStat->GetCurrentStat() <= 0.f)
	{
		PlayerAttackComponent->StopAttack();
		PlayerAttackComponent->Attack(EPlayerAttackType::Dead);
		DisableInput(Cast<APlayerController>(GetController()));
		bDead = true;
		if (GameOverWidget)
		{
			RemoveFromViewPort(MainWidgetInterface);
			AddToViewPort(GameOverWidget);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GameOverWidget is null"));
		}
	}
	return true;
}

bool ABasePlayer::IsDead_Implementation() const
{
	return bDead;
}

bool ABasePlayer::KnockBackAttack_Implementation(const TScriptInterface<IAttackInterface>& Other)
{
	if (!KnockBackAttack && !bDead)
	{
		PlayerAttackComponent->StopAttack();
		PlayerAttackComponent->OnPlayerAttackEnd.AddUniqueDynamic(this, &ABasePlayer::KnockBackReset);
		PlayerAttackComponent->Attack(EPlayerAttackType::KnockBack);
		KnockBackAttack = true;
	}
	return KnockBackAttack;
}

bool ABasePlayer::HitReaction_Implementation(const TScriptInterface<IAttackInterface>& Other)
{
	if (PlayerAttackComponent->IsHitted() || bDead)
		return false;
	PlayerAttackComponent->StopAttack();
	EPlayerAttackType Hits[3] = { EPlayerAttackType::Hit01, EPlayerAttackType::Hit02, EPlayerAttackType::Hit03 };
	int32 RandIndex = FMath::Rand32() % 1;
	PlayerAttackComponent->Attack(Hits[RandIndex]);
	return true;
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
	if(StatComponent_Level)
		StatComponent_Level->AddCurrentStat(35.0);
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

bool ABasePlayer::HasMainWidget() const
{
	return MainWidgetInterface != nullptr;
}

bool ABasePlayer::AddToViewPort(TScriptInterface<IMainWidgetInterface> NewWidget)
{
	if (MainWidgetInterface || !NewWidget)
		return false;
	MainWidgetInterface = NewWidget;
	return MainWidgetInterface->SetMainWidget();
}

void ABasePlayer::RemoveFromViewPort(TScriptInterface<IMainWidgetInterface> NewWidget)
{
	if (MainWidgetInterface != NewWidget)
		return;
	if (MainWidgetInterface)
	{
		MainWidgetInterface->UnSetMainWidget();
	}
	MainWidgetInterface = nullptr;
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
		if (CurrentPlayerState == EPlayerState::TopDown)
			break;
		//if (!PlayerAnimationComponent || !PlayerAnimationComponent->StartClimb())
		if(!PlayerAttackComponent->IsAttacking())
		{
			Jump();
		}
		break;
	case EInputKeyType::MouseAxis:
		break;
	case EInputKeyType::KeyF:
		if (InteractionComponent && BuildAssistComponent && !BuildAssistComponent->IsBuildingActive() || !HasMainWidget())
		{
			InteractionComponent->OnInteractionStart();
			//AddToViewPort(InteractionComponent);
		}
		break;
	case EInputKeyType::Esc:
		break;
	case EInputKeyType::KeyC:
		break;
	case EInputKeyType::MouseR:
		if (BuildAssistComponent && CurrentPlayerState != EPlayerState::TopDown && InteractionComponent && !InteractionComponent->IsInteracting())
		{
			BuildAssistComponent->SpawnBuilding();
		}
		break;
	case EInputKeyType::MouseL:
		if (BuildAssistComponent && CurrentPlayerState != EPlayerState::TopDown && InteractionComponent && !InteractionComponent->IsInteracting())
		{
			BuildAssistComponent->SpawnBuilding();
			BuildAssistComponent->EndBuilding();
			//RemoveFromViewPort(MainWidgetInterface);
		}
		break;
	case EInputKeyType::MouseWheel:
		break;
	case EInputKeyType::KeyB:
		break;
	case EInputKeyType::KeyTab:
	{
		//if (InteractionComponent && BuildAssistComponent && !BuildAssistComponent->IsBuildingActive() && !InteractionComponent->IsInteracting())
		//{
			if (BasePlayerController)
			{
				if (!AddToViewPort(BasePlayerController->GetInventoryWidget()))
				{
					RemoveFromViewPort(BasePlayerController->GetInventoryWidget());
				}
			}
		//}
		break;
	}
	default:
		break;
	}
}

void ABasePlayer::TriggerEvent(const FInputActionValue& Value, EInputKeyType KeyType)
{
	switch (KeyType)
	{
	case EInputKeyType::WASD:
		if (PlayerMoveFunc && CurrentPlayerState != EPlayerState::TopDown)
		{
			(this->*PlayerMoveFunc)(Value);
		}
		break;
	case EInputKeyType::SpaceBar:
		break;
	case EInputKeyType::MouseAxis:
		if (CurrentPlayerState != EPlayerState::TopDown)
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
		if (InteractionComponent && BuildAssistComponent && !BuildAssistComponent->IsBuildingActive())
		{
			InteractionComponent->OnInteractionTriggered();
		}
		break;
	case EInputKeyType::Esc:
		break;
	case EInputKeyType::KeyC:
		if (InteractionComponent && BuildAssistComponent && !BuildAssistComponent->IsBuildingActive() )
		{
			InteractionComponent->OnActorCancel();
		}
		break;
	case EInputKeyType::MouseR:
		break;
	case EInputKeyType::MouseL:
		if (CurrentPlayerState == EPlayerState::TopDown)
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
		if (BuildAssistComponent && InteractionComponent && !InteractionComponent->IsInteracting())
		{
			BuildAssistComponent->RotateBuilding(LookAxisVector.X * 5.0f);
		}
	}
		break;
	case EInputKeyType::KeyB:
	{
		break;
	}
	case EInputKeyType::KeyTab:
	{
		break;
	}
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
		if (CurrentPlayerState != EPlayerState::TopDown)
		{
			StopJumping();
		}
		break;
	case EInputKeyType::MouseAxis:
		break;
	case EInputKeyType::KeyF:
		if (InteractionComponent && BuildAssistComponent && !BuildAssistComponent->IsBuildingActive() && !BasePlayerController->bIsInventoryOpen())
		{
			InteractionComponent->OnInteractionCompleted();
		}
		break;
	case EInputKeyType::Esc:
	{
		if (HasMainWidget())
		{
			RemoveFromViewPort(MainWidgetInterface);
		}
		//if (BuildAssistComponent && InteractionComponent && !InteractionComponent->IsInteracting())
		//{
		//	BuildAssistComponent->EndBuilding();
		//}
		if (InteractionComponent && BuildAssistComponent && !BuildAssistComponent->IsBuildingActive())
		{
			InteractionComponent->OnInteractionCompleted();
		}
		break;
	}
	case EInputKeyType::KeyC:
		break;
	case EInputKeyType::MouseR:
		break;
	case EInputKeyType::MouseL:
		if (CurrentPlayerState == EPlayerState::Battle)
		{
			if (PlayerAttackComponent)
			{
				PlayerAttackComponent->Attack(EPlayerAttackType::Default);
			}
		}
		break;
	case EInputKeyType::MouseWheel:
		break;
	case EInputKeyType::KeyB:
	{
		if (CurrentPlayerState != EPlayerState::TopDown)
		{
			if (BuildAssistComponent && !AddToViewPort(BuildAssistComponent))
			{
				RemoveFromViewPort(BuildAssistComponent);
				//BuildAssistComponent->EndBuilding();
			}
		}
		break;
	}
	case EInputKeyType::Key1:
	case EInputKeyType::Key2:
	case EInputKeyType::Key3:
	{
		if (PlayerAttackComponent)
		{
			int32 SkillIndex = (int32)KeyType - (int32)EInputKeyType::Key1 + (int32)EPlayerAttackType::Skill01;
			PlayerAttackComponent->Attack((EPlayerAttackType)(SkillIndex));
			if (PlayerAttackComponent->IsAttacking())
			{
				RemoveFromViewPort(MainWidgetInterface);
			}
		}
		break;
	}
	case EInputKeyType::KeyTab:
	{
		break;
	}
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

void ABasePlayer::Restart()
{
	Super::Restart();
	if (bDead)
	{
		bDead = false;
		RemoveFromViewPort(GameOverWidget);
		HPStat->SetCurrentStat(HPStat->GetMaxStat());
		if (PlayerAttackComponent)
			PlayerAttackComponent->StopAttack();
		EnableInput(Cast<APlayerController>(GetController()));
		if(CurrentPlayerState == EPlayerState::TopDown)
		{
			APlayerController* PlayerController = Cast<APlayerController>(GetController());
			if (PlayerController)
			{
				PlayerController->SetShowMouseCursor(true);
			}
		}
	}
}

void ABasePlayer::KnockBackReset()
{
	KnockBackAttack = false;
	PlayerAttackComponent->OnPlayerAttackEnd.RemoveDynamic(this, &ABasePlayer::RetAttackValue);
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
		TArray<FPlayerInputData*> ArrActions{};
		if (InputDataTable)
		{
			InputDataTable->GetAllRows(TEXT(""), ArrActions);
		}
		int32 ArrCount = ArrActions.Num();;
		for (int32 i = 0; i < ArrCount; ++i)
		{
			if (FPlayerInputData*  InputActionData =  ArrActions[i])
			{
				EnhancedInputComponent->BindAction(InputActionData->InputAction, ETriggerEvent::Started, this, &ABasePlayer::OnStartEvent, InputActionData->InputKeyType);
				EnhancedInputComponent->BindAction(InputActionData->InputAction, ETriggerEvent::Triggered, this, &ABasePlayer::OnTriggerEvent, InputActionData->InputKeyType);
				EnhancedInputComponent->BindAction(InputActionData->InputAction, ETriggerEvent::Completed, this, &ABasePlayer::OnCompleteEvent, InputActionData->InputKeyType);
				ResetDeaflut(InputActionData->InputKeyType);
			}
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

void ABasePlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	BasePlayerController = Cast<ABasePlayerController>(NewController);
}
