#pragma once

#include "CoreMinimal.h"
#include "GameBase/BaseCharacter.h"
#include "Logging/LogMacros.h"
#include "Building/Interface/ArchitectureInterface.h"
#include "Resource/Interface/ResourceInterface.h"
#include "Player/Interface/PlayerInputInterface.h"
#include "BasePlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UInteractionComponent;
class UBuildingAssistComponent;
class UPlayerAnimationComponent;
class UBuildingModeWidget;
class UNavigationInvokerComponent;
class UPlayerAttackComponent;
class UPlayerDetectCollision;
class UPlayerItemComponent;
class ABasePlayerController;
class UStatComponent_Level;
class IMainWidgetInterface;
class UPlayerGameOver;
class UMonsterSpawnerComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogBasePlayer, Log, All);

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	Travel UMETA(DisplayName = "Default"),
	Climb,
	Battle,
	TopDown,
	Mining,
	EnumMax UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EStatusType : uint8
{
	None UMETA(Hidden),
	Hp,
	MaxHp,
	Shield,
	MaxShield,
	Health,
	MaxHealth,
	Stamina,
	Attack,
	Defense,
	WorkSpeed,
	MaxWeight,
	EnumMax UMETA(Hidden)
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChange, EPlayerState , PrePlayerState);

UCLASS()
class PROJECTOPENWORLD_API ABasePlayer : public ABaseCharacter, public IArchitectureInterface, public IResourceInterface
	, public IPlayerInputInterface, public IPlayerInputSettingInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<USpringArmComponent> CameraBoom{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr <UPlayerItemComponent> PlayerItemManagerComponent{};
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr< UCameraComponent> FollowCamera{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr <UStatComponent_Level> StatComponent_Level{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr <USkeletalMeshComponent> WeaponMeshComponent{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Building)
	TObjectPtr<UBuildingAssistComponent> BuildAssistComponent{}; 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr < UPlayerAnimationComponent>	PlayerAnimationComponent{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<UNavigationInvokerComponent> NavigationInvokerComp{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<UMonsterSpawnerComponent> MonsterSpawnerComponent{};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr < UInputMappingContext> DefaultMappingContext{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr < UDataTable> InputDataTable{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction)
	TObjectPtr<UInteractionComponent> InteractionComponent{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Interaction)
	TArray<float> StatusArray{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Interaction)
	UAnimMontage* ClimbMontage{};

	void (ABasePlayer::*PlayerMoveFunc)(const FInputActionValue&);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EPlayerState CurrentPlayerState{};
	UPROPERTY()
	TObjectPtr< UBuildingModeWidget> BuildingWidget{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AttackMontage")
	TObjectPtr< UPlayerAttackComponent> PlayerAttackComponent{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AttackMontage")
	TObjectPtr< UPlayerDetectCollision> PlayerDetectCollision{};
	
	TMap<EInputKeyType, TScriptInterface<IPlayerInputInterface>> InputMapping{};

	TSubclassOf<UPlayerGameOver> GameOverWidgetClass{};
	UPROPERTY()
	TObjectPtr<UPlayerGameOver> GameOverWidget{};

	TObjectPtr< ABasePlayerController> BasePlayerController{};

	TScriptInterface< IMainWidgetInterface> MainWidgetInterface{};

public:

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Damaged")
	FOnDamageedDelegate OnDamagedDelegate{};

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "PlayerState")
	FOnPlayerStateChange OnStateChangeDelegate{};
	bool TopDownMode{};
	bool KnockBackAttack{};
public:
	ABasePlayer();


	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "PlayerMode")
	void SetTopDownMode(bool bTopDown);
	UFUNCTION(BlueprintCallable, Category = "PlayerMode")
	void StartClimb();
	UFUNCTION(BlueprintPure, Category = "PlayerMode")
	bool IsTopDownMode() const { return CurrentPlayerState == EPlayerState::TopDown; }

	UFUNCTION(BlueprintCallable )
	void StartTravel();

	void UpdateWeight(float InventoryWeight);

	UFUNCTION()
	void OnStartEvent(const FInputActionValue& Value, EInputKeyType KeyType);
	UFUNCTION()
	void OnTriggerEvent(const FInputActionValue& Value, EInputKeyType KeyType);
	UFUNCTION()
	void OnCompleteEvent(const FInputActionValue& Value, EInputKeyType KeyType);

	// IPlayerInputInterface interface
	virtual void StartEvent(const FInputActionValue& Value, EInputKeyType KeyType) override;
	virtual void TriggerEvent(const FInputActionValue& Value, EInputKeyType KeyType) override;
	virtual void CompleteEvent(const FInputActionValue& Value, EInputKeyType KeyType) override;
	virtual void SetInputInterface(EInputKeyType KeyType, TScriptInterface<IPlayerInputInterface> InputInterface) override;
	virtual void ResetDeaflut(EInputKeyType KeyType) override;

	virtual void Restart() override;
protected:
	UFUNCTION()
	void KnockBackReset();
	///** Called for movement input */
	void MoveClimb(const FInputActionValue& Value);
	void MoveTravel(const FInputActionValue& Value);
	virtual void PossessedBy(AController* NewController) override;
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay();
	UFUNCTION()
	void OnLevelUpEvent(int32 OldLevel, bool IsMaxLevel);

public:
	FORCEINLINE UPlayerAttackComponent* const GetPlayerAttackComponent() const { return PlayerAttackComponent; }
	FORCEINLINE EPlayerState GetPlayerState() const { return CurrentPlayerState; }  //
	/** Returns CameraBoom subobject **/
	FORCEINLINE  USpringArmComponent* const  GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE  UCameraComponent* const GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE  UBuildingAssistComponent* const GetBuildingAssist() const { return BuildAssistComponent; }
	FORCEINLINE  UInteractionComponent* const GetInteractionComponent() const { return InteractionComponent; }
	FORCEINLINE  USkeletalMeshComponent* const GetWeaponMeshComponent() const { return WeaponMeshComponent; }
	FORCEINLINE UPlayerItemComponent* const GetPlayerItemComponent() const { return PlayerItemManagerComponent; }
	FORCEINLINE UStatComponent_Level* const GetLevelComponent() const { return StatComponent_Level; }

	UFUNCTION(BlueprintPure, Category = "PlayerAnimation")
	FORCEINLINE  UPlayerAnimationComponent* const GetPlayerAnimationComponent() const { return PlayerAnimationComponent; }
	

	void SetWeaponMesh(USkeletalMesh* NewMesh, FName SocketName);

	FORCEINLINE  float* GetStatusRef(EStatusType StatusType) {
		return  StatusArray.IsValidIndex((uint8)StatusType) ? &StatusArray[(uint8)StatusType] : &StatusArray[0];
	} 

	UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
	void SetStatus(EStatusType StatusType, float Value);
	UFUNCTION(BlueprintPure, Category = "PlayerStatus")
	bool GetStatus(EStatusType StatusType, float& Result) const;
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void ChangePlayerState(EPlayerState NewState);


	virtual float GetAttackValue_Implementation() const override;
	virtual void  SetAttackValue_Implementation(float NewValue) override;
	virtual void  RetAttackValue_Implementation() override;
	virtual bool DamagedCharacter_Implementation(const TScriptInterface< IAttackInterface>& Other) override;
	virtual bool IsDead_Implementation() const override;
	virtual bool KnockBackAttack_Implementation(const TScriptInterface< IAttackInterface>& Other) override;
	virtual bool HitReaction_Implementation(const TScriptInterface< IAttackInterface>& Other) override;

	virtual float GetArchitectSpeed_Implementation() const override;
	virtual void StartArchitect_Implementation(ABaseBuilding* Building) override;
	virtual void StopArchitect_Implementation(ABaseBuilding* Building) override;
	virtual void EndArchitect_Implementation(ABaseBuilding* Building) override;

	virtual float GetResourceSpeed_Implementation() const override;
	virtual void StartResource_Implementation(AResourceActor* ResourceActor) override;
	virtual void StopResource_Implementation(AResourceActor* ResourceActor) override;
	virtual void EndResource_Implementation(AResourceActor* ResourceActor) override;
	
	bool HasMainWidget() const;
	bool AddToViewPort(TScriptInterface< IMainWidgetInterface> NewWidget);
	void RemoveFromViewPort(TScriptInterface< IMainWidgetInterface> NewWidget);

};

