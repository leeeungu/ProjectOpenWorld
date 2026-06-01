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
#include "Building/BaseBuilding.h"
#include "Player/Component/PlayerMoveComponent.h"
#include "Player/Component/PlayerEquipComponent.h"
#include "Player/Widget/MainUI.h"
#include "Item/Component/ItemUseComponent.h"
#include "Player/Component/PlayerEquipVisualComponent.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Pal/Component/PalHitHandlerComponent.h"
#include "Pal/Data/PalDamageType.h"

DEFINE_LOG_CATEGORY(LogBasePlayer);

ABasePlayer::ABasePlayer() : Super{}
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
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	InteractionComponent = CreateDefaultSubobject<UPlayerInteractionComponent>(TEXT("InteractionComponent"));

	BuildAssistComponent = CreateDefaultSubobject<UBuildingAssistComponentV2>(TEXT("BuildingAssist"));

	PlayerAnimationComponent = CreateDefaultSubobject<UPlayerAnimationComponent>(TEXT("PlayerAnimationComponent"));
//	StatusArray.Init(0, (uint8)EStatusType::EnumMax);
	//PlayerMoveFunc = &ABasePlayer::MoveTravel;

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
	PlayerDetectCollision->SetupAttachment(GetRootComponent());

	PlayerItemManagerComponent = CreateDefaultSubobject<UPlayerItemComponent>(TEXT("PlayerItemManagerComponent"));
	StatComponent_Level = CreateDefaultSubobject<UStatComponent_Level>(TEXT("StatComponent_Level"));

	//MonsterSpawnerComponent = CreateDefaultSubobject<UMonsterSpawnerComponent>(TEXT("MonsterSpawnerComponent"));
	//MonsterSpawnerComponent->SetupAttachment(GetRootComponent());

	PlayerMoveComponent = CreateDefaultSubobject<UPlayerMoveComponent>(TEXT("PlayerMoveComponent"));

	PlayerEquipComponent = CreateDefaultSubobject<UPlayerEquipComponent>(TEXT("PlayerEquipComponent"));
	PlayerItemUseComponent = CreateDefaultSubobject<UItemUseComponent>(TEXT("PlayerItemUseComponent"));

	RightHandEquipComponent = CreateDefaultSubobject<UPlayerEquipVisualComponent>(TEXT("RightHandEquipComponent"));
	RightHandEquipComponent->SetupAttachment(GetMesh());
	LeftHandEquipComponent = CreateDefaultSubobject<UPlayerEquipVisualComponent>(TEXT("LeftHandEquipComponent"));
	LeftHandEquipComponent->SetupAttachment(GetMesh());

	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("PlayerStatCom"));
	HitHandlerComponent = CreateDefaultSubobject<UPalHitHandlerComponent>(TEXT("HitHandlerComponent"));
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

//void ABasePlayer::StartClimb()
//{
//	if (PlayerAnimationComponent->StartClimb())
//		PlayerMoveFunc = &ABasePlayer::MoveClimb;
//}
//
//void ABasePlayer::StartTravel()
//{
//	if(PlayerAnimationComponent->StartTravel())
//		PlayerMoveFunc = &ABasePlayer::MoveTravel;
//}

void ABasePlayer::UpdateWeight(float InventoryWeight)
{
	/*if (*GetStatusRef(EStatusType::MaxWeight) > InventoryWeight)
	{
		GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 50.0f;
	}*/
}


void ABasePlayer::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		GameOverWidget = CreateWidget<UPlayerGameOver>(PlayerController, GameOverWidgetClass);
	}

	if (HitHandlerComponent && StatComponent)
	{
		HitHandlerComponent->OnDamageTaken.AddUniqueDynamic(StatComponent, &UStatComponent::OnReceiveDamage);
	}
	if (StatComponent)
	{
		if (FOnStatChanged* HpChanged = StatComponent->GetCurrentOnStatChanged(EStatusType::HP))
		{
			HpChanged->AddUniqueDynamic(this, &ABasePlayer::OnHPChanged);
		}
	}

	if(StatComponent_Level)
		StatComponent_Level->OnLevelUp.AddUniqueDynamic(this, &ABasePlayer::OnLevelUpEvent);

	SetInputInterface(EInputKeyType::WASD, PlayerMoveComponent);
	SetInputInterface(EInputKeyType::MouseWheel, PlayerEquipComponent);
	
	if (MainWidgetClass)
	{
		MainWidget = CreateWidget<UUserWidget>(PlayerController, MainWidgetClass);
		if (MainWidget)
		{
			MainWidget->AddToViewport();
		}
		if (UMainUI* _MainWidget = Cast< UMainUI>(MainWidget))
		{
			_MainWidget->SetPlayerStatWidget(StatComponent);
		}
	}

	if(PlayerDetectCollision)
	{
		PlayerDetectCollision->OnDetectChanged.AddDynamic(this, &ABasePlayer::OnMonsterDetectChanged);
	}
}

void ABasePlayer::OnLevelUpEvent(int32 OldLevel, bool IsMaxLevel)
{
	if (IsMaxLevel)
		return;
	//float MaxHp = *GetStatusRef(EStatusType::MaxHp);
	//float NewMaxHp = MaxHp + 50;
	////SetStatus(EStatusType::MaxHp, NewMaxHp);
	//HPStat->SetMaxStat(NewMaxHp);
	//HPStat->SetCurrentStat(NewMaxHp);
	//AttackStat->AddCurrentStat(20.0f);
}

UEquipmentComponent* const ABasePlayer::GetPlayerEquipComponent() const
{
	return PlayerEquipComponent;
}

UPlayerAnimInstance* const ABasePlayer::GetPlayerAnimInstance() const
{
	if (GetMesh())
	{
		return Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	}
	return nullptr;
}

USkeletalMeshComponent* ABasePlayer::GetRightHandEquipComponent() const
{
	return RightHandEquipComponent;
}

USkeletalMeshComponent* ABasePlayer::GetLeftHandEquipComponent() const
{
	return LeftHandEquipComponent;
}
class UInventoryComponent* ABasePlayer::GetInventoryComponent() const
{
	if (BasePlayerController)
	{
		return BasePlayerController->GetInventoryComponent();
	}
	return nullptr;
}

void ABasePlayer::SetWeaponMesh(USkeletalMesh* NewMesh, FName SocketName)
{
	//if (PlayerEquipComponent)
	//	PlayerEquipComponent->SetEquipMesh(NewMesh);
	//{
	//	PlayerEquipComponent->SetSkeletalMesh(NewMesh);
	//	PlayerEquipComponent->SetRelativeTransform(FTransform::Identity);
	//	PlayerEquipComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketName);
	//}
}

void ABasePlayer::UnEquip(USkeletalMesh* OldMesh)
{
	//if (PlayerEquipComponent)
	//	PlayerEquipComponent->SetUnequipMesh(OldMesh);
}

void ABasePlayer::SetStatus(EStatusType StatusType, float Value)
{
	if (StatComponent)
	{
		StatComponent->SetCurrentStat(Value, StatusType);

	}
}

bool ABasePlayer::GetStatus(EStatusType StatusType, float& Result) const
{
	Result = StatComponent->GetCurrentStat(StatusType);
	return true;
	/*switch (StatusType)
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
	*/
	//return false;
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
		InteractionComponent->SetInteractionable(true);
		break;
	}
	case EPlayerState::Building:
	{
		SetInputInterface(EInputKeyType::MouseL, this);
		SetInputInterface(EInputKeyType::MouseR, this);
		SetInputInterface(EInputKeyType::MouseWheel, PlayerEquipComponent);
		SetInputInterface(EInputKeyType::Esc, this);
		InteractionComponent->SetInteractionable(true);
		RemoveFromViewPort(BuildAssistComponent);
		break;
	}
	case EPlayerState::EnumMax:
		break;
	default:
		break;
	}

	CurrentPlayerState = NewState;
	switch (CurrentPlayerState)
	{
	case EPlayerState::Travel:
	{
		//UE_LOG(LogBasePlayer, Log, TEXT("Travel Mode"));
		//if (PlayerAnimationComponent->StartTravel())
		//	PlayerMoveFunc = &ABasePlayer::MoveTravel;
		UseOrientRotationToMovement();
		PlayerMoveComponent->SetDefaultMove();
		break;
	}
	case EPlayerState::Climb:
	{
		//if (PlayerAnimationComponent->StartClimb())
		//	PlayerMoveFunc = &ABasePlayer::MoveClimb;
		break;
	}
	case EPlayerState::Battle:
		//UE_LOG(LogBasePlayer, Log, TEXT("Battle Mode"));
		PlayerMoveComponent->SetSwordMove();
		UseControllerDesiredRotation();
		break;
	case EPlayerState::TopDown:
	{
		TopDownMode = true;
		UseOrientRotationToMovement();

		PlayerMoveComponent->SetTopDownMode();
		BuildAssistComponent->EndBuilding();
		InteractionComponent->SetInteractionable(false);
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (PlayerController)
		{
			PlayerController->SetShowMouseCursor(true);
		}
		break;
	}
	case EPlayerState::Building:
	{
		if (!AddToViewPort(BuildAssistComponent))
		{
			ChangePlayerState(PrevState);
			return;
		}
		else
		{
			SetInputInterface(EInputKeyType::MouseL, BuildAssistComponent);
			SetInputInterface(EInputKeyType::MouseR, BuildAssistComponent);
			SetInputInterface(EInputKeyType::MouseWheel, BuildAssistComponent);
			SetInputInterface(EInputKeyType::Esc, BuildAssistComponent);
			InteractionComponent->SetInteractionable(false);
		}
		break;
	}
	case EPlayerState::EnumMax:
		break;

	default:
		break;
	}
	if (OnStateChangeDelegate.IsBound() && CurrentPlayerState != PrevState)
	{
		OnStateChangeDelegate.Broadcast(CurrentPlayerState, PrevState);
	}
}

void ABasePlayer::ChangePlayerEquip(FName WeaponName, EWeapone NewEquip)
{
	UE_LOG(LogBasePlayer, Log, TEXT("UnUsedFunction"));
}

void ABasePlayer::ChangeEquipWidget(FName WeaponName, EWeapone NewEquip)
{
	if (MainWidget)
	{
		UMainUI* WeaponeWidget = Cast<UMainUI>(MainWidget);
		if (WeaponeWidget)
		{
			WeaponeWidget->ChangeWeapone(WeaponName, NewEquip);
		}
	}
}

void ABasePlayer::SetMonsterSpawner(bool bActive)
{
	//if(MonsterSpawnerComponent)
	//	MonsterSpawnerComponent->SetSpawnable(bActive);
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

bool ABasePlayer::DamagedCharacter_Implementation(const TScriptInterface<IAttackInterface>& Other)
{
	return false;
}

void ABasePlayer::OnHPChanged(double PreCurrentStat, double CurrentStat)
{
	if (bDead || CurrentStat > 0.0)
		return;

	bDead = true;
	if (PlayerAttackComponent)
	{
		PlayerAttackComponent->StopAttack();
		PlayerAttackComponent->Attack(EPlayerAttackType::Dead);
	}
	DisableInput(Cast<APlayerController>(GetController()));

	if (GameOverWidget)
	{
		RemoveFromViewPort(MainWidgetInterface);
		AddToViewPort(GameOverWidget);
	}
	else
	{
		UE_LOG(LogBasePlayer, Warning, TEXT("GameOverWidget is null"));
	}
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
	if (GetPlayerAnimInstance()->SetArchitectAnimSequence())
	{
		GetPlayerAnimInstance()->StartAnimSection();
	}
}

void ABasePlayer::StopArchitect_Implementation(ABaseBuilding* Building)
{
	GetPlayerAnimInstance()->EndAnimSection();
}

void ABasePlayer::EndArchitect_Implementation(ABaseBuilding* Building)
{
	if(StatComponent_Level)
		StatComponent_Level->AddCurrentStat(35.0);
	GetPlayerAnimInstance()->EndAnimSection();
}

float ABasePlayer::GetResourceSpeed_Implementation() const
{
	return 1.0f;
}

void ABasePlayer::StartResource_Implementation(AResourceActor* ResourceActor)
{
	GetPlayerAnimInstance()->StartAnimSection();
}

void ABasePlayer::StopResource_Implementation(AResourceActor* ResourceActor)
{
	GetPlayerAnimInstance()->EndAnimSection();
}

void ABasePlayer::EndResource_Implementation(AResourceActor* ResourceActor)
{
	GetPlayerAnimInstance()->EndAnimSection();
}

bool ABasePlayer::HasMainWidget() const
{
	return MainWidgetInterface != nullptr;
}

bool ABasePlayer::AddToViewPort(TScriptInterface<IMainWidgetInterface> NewWidget)
{
	if (MainWidgetInterface)
	{
		UE_LOG(LogBasePlayer, Warning, TEXT("MainWidgetInterface is already set"));
		return false;
	}
	if(!NewWidget)
	{
		UE_LOG(LogBasePlayer, Warning, TEXT("NewWidget is null"));
		return false;
	}
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

void ABasePlayer::OnMonsterDetectChanged(AActor* Actor, bool bDetected)
{
	if (UMainUI* _MainWidget = Cast< UMainUI>(MainWidget))
	{
		if (bDetected)
		{
			_MainWidget->RegisterMonster(Actor);
		}
		else
		{
			_MainWidget->UnregisterMonster(Actor);
		}
	}
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
	/*case EInputKeyType::WASD:
		break;*/
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
	
		break;
	case EInputKeyType::MouseL:
		if (CurrentPlayerState != EPlayerState::Battle)
		{
			if (GetPlayerAnimInstance())
				GetPlayerAnimInstance()->StartAnimSection();
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
	/*case EInputKeyType::WASD:
		if (PlayerMoveFunc && CurrentPlayerState != EPlayerState::TopDown)
		{
			(this->*PlayerMoveFunc)(Value);
		}
		break;*/
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
	{

		break;
	}
	case EInputKeyType::MouseWheel:
		break;
	case EInputKeyType::KeyB:
		break;
	case EInputKeyType::KeyTab:
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
	//	if (PlayerMoveFunc == &ABasePlayer::MoveClimb)
	//	{
	//		if (UPlayerAnimInstance* Instance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	//		{
	//			Instance->SetClimbSpeed(0);
	//		}
	//	}
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
		if (BasePlayerController && InteractionComponent && BuildAssistComponent && !BuildAssistComponent->IsBuildingActive() && !BasePlayerController->bIsInventoryOpen())
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
		else
		{
			if(GetPlayerAnimInstance())
				GetPlayerAnimInstance()->EndAnimSection();
		}
		break;
	case EInputKeyType::MouseWheel:
		break;
	case EInputKeyType::KeyB:
	{
		if (CurrentPlayerState != EPlayerState::TopDown)
		{
			if (CurrentPlayerState == EPlayerState::Building)
			{
				ChangePlayerState(EPlayerState::Travel);
			}
			else
				ChangePlayerState(EPlayerState::Building);
		}
		break;
	}
	case EInputKeyType::Key1:
	case EInputKeyType::Key2:
	case EInputKeyType::Key3:
	{
		if (PlayerAttackComponent && CurrentPlayerState == EPlayerState::Battle)
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
	InputMapping.FindOrAdd(KeyType, nullptr) = InputInterface;
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
		if(StatComponent)
			StatComponent->StatBeginPlay(EStatusType::HP);
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

void ABasePlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	BasePlayerController = Cast<ABasePlayerController>(NewController);
}