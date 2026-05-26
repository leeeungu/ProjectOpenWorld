#pragma once

#include "CoreMinimal.h"
#include "Pal/Character/PalBaseCharacter.h"
#include "Logging/LogMacros.h"
#include "Building/Interface/ArchitectureInterface.h"
#include "Resource/Interface/ResourceInterface.h"
#include "Player/Interface/PlayerInputInterface.h"
#include "GameBase/Interface/AttackInterface.h"
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
class UPlayerMoveComponent;
enum class EWeapone : uint8;
class UUserWidget;
class UPlayerEquipComponent;
class UItemUseComponent;
class UEquipmentComponent;
class UPlayerEquipVisualComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogBasePlayer, Log, All);

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	Travel UMETA(DisplayName = "Default"),
	Climb,
	Battle,
	TopDown,
	Mining,
	Building,
	EnumMax UMETA(Hidden)
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerStateChange, EPlayerState, NewPlayerState, EPlayerState, PrePlayerState);

UCLASS()
class PROJECTOPENWORLD_API ABasePlayer : public APalBaseCharacter, public IArchitectureInterface, public IResourceInterface
	, public IPlayerInputInterface, public IPlayerInputSettingInterface, public IAttackInterface
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
	UPROPERTY(VisibleAnywhere,  Category = Camera)
	TObjectPtr< UStatComponent> PlayerStatComponent{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Building)
	TObjectPtr<UBuildingAssistComponent> BuildAssistComponent{}; 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr < UPlayerAnimationComponent>	PlayerAnimationComponent{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<UNavigationInvokerComponent> NavigationInvokerComp{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<UPlayerEquipVisualComponent> RightHandEquipComponent{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<UPlayerEquipVisualComponent> LeftHandEquipComponent{};

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
	//TObjectPtr<UMonsterSpawnerComponent> MonsterSpawnerComponent{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr< UPlayerMoveComponent> PlayerMoveComponent{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr< UPlayerEquipComponent> PlayerEquipComponent{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr< UItemUseComponent> PlayerItemUseComponent{};
	
	
	
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

	//void (ABasePlayer::*PlayerMoveFunc)(const FInputActionValue&);
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	EWeapone CurrentEquip{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MainWidget")
	TSubclassOf<UUserWidget> MainWidgetClass{};
	TObjectPtr<UUserWidget> MainWidget{};

	bool bDead{};
public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "PlayerState")
	FOnPlayerStateChange OnStateChangeDelegate{};
	bool TopDownMode{};
	bool KnockBackAttack{};
public:
	ABasePlayer();


	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "PlayerMode")
	void SetTopDownMode(bool bTopDown);
	//UFUNCTION(BlueprintCallable, Category = "PlayerMode")
	//void StartClimb();
	UFUNCTION(BlueprintPure, Category = "PlayerMode")
	bool IsTopDownMode() const { return CurrentPlayerState == EPlayerState::TopDown; }

	//UFUNCTION(BlueprintCallable )
	//void StartTravel();

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
	//void MoveTravel(const FInputActionValue& Value);
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
	FORCEINLINE UPlayerItemComponent* const GetPlayerItemComponent() const { return PlayerItemManagerComponent; }
	FORCEINLINE UStatComponent_Level* const GetLevelComponent() const { return StatComponent_Level; }
	//FORCEINLINE UMonsterSpawnerComponent* const GetMonsterSpawnerComponent() const { return MonsterSpawnerComponent; }
	FORCEINLINE UItemUseComponent* const GetPlayerItemUseComponent() const { return PlayerItemUseComponent; }
	UEquipmentComponent* const GetPlayerEquipComponent() const;
	class UPlayerAnimInstance* const GetPlayerAnimInstance() const;
	USkeletalMeshComponent* GetRightHandEquipComponent() const;
	USkeletalMeshComponent* GetLeftHandEquipComponent() const;
	class UInventoryComponent* GetInventoryComponent() const;
	
	UFUNCTION(BlueprintPure, Category = "PlayerAnimation")
	FORCEINLINE  UPlayerAnimationComponent* const GetPlayerAnimationComponent() const { return PlayerAnimationComponent; }
	

	void SetWeaponMesh(USkeletalMesh* NewMesh, FName SocketName  = NAME_None);
	void UnEquip(USkeletalMesh* OldMesh);

	FORCEINLINE  float* GetStatusRef(EStatusType StatusType) {
		return  StatusArray.IsValidIndex((uint8)StatusType) ? &StatusArray[(uint8)StatusType] : &StatusArray[0];
	} 

	UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
	void SetStatus(EStatusType StatusType, float Value);
	UFUNCTION(BlueprintPure, Category = "PlayerStatus")
	bool GetStatus(EStatusType StatusType, float& Result) const;
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void ChangePlayerState(EPlayerState NewState);
	void ChangePlayerEquip(FName WeaponName, EWeapone NewEquip);
	// 좌하단 장비 UI
	void ChangeEquipWidget(FName WeaponName, EWeapone NewEquip);

	void SetMonsterSpawner(bool bActive);

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

