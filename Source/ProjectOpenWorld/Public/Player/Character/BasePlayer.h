#pragma once

#include "CoreMinimal.h"
#include "GameBase/BaseCharacter.h"
#include "Logging/LogMacros.h"
#include "BasePlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UInteractionComponent;
class UBuildingAssistComponent;
class UPlayerAnimationComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogBasePlayer, Log, All);

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

UCLASS()
class PROJECTOPENWORLD_API ABasePlayer : public ABaseCharacter
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom{};

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr< UCameraComponent> FollowCamera{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Building, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBuildingAssistComponent> BuildAssistComponent{}; 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TObjectPtr < UPlayerAnimationComponent>	PlayerAnimationComponent{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext{};

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction{};

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction{};

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractionAction{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EscAction{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* KeyCAction{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MouseRAction{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MouseLAction{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MouseWheelAction{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInteractionComponent> InteractionComponent{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	TArray<float> StatusArray{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ClimbMontage{};
	void (ABasePlayer::*PlayerMoveFunc)(const FInputActionValue&);
public:
	ABasePlayer();


	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable )
	void StartClimb();
	UFUNCTION(BlueprintCallable )
	void StartTravel();

	void UpdateWeight(float InventoryWeight);
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	void OnMoveCompleted(const FInputActionValue& Value);
	void MoveClimb(const FInputActionValue& Value);
	void MoveTravel(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);


	UFUNCTION()
	void OnInteractionStart(const FInputActionValue& Value);
	UFUNCTION()
	void OnInteraction(const FInputActionValue& Value);
	UFUNCTION()
	void OnInteractionEnd(const FInputActionValue& Value);
	UFUNCTION()
	void OnActionExit(const FInputActionValue& Value);
	UFUNCTION()
	void OnActionKeyC(const FInputActionValue& Value);
	UFUNCTION()
	void OnActionMouseWheel(const FInputActionValue& Value);
	UFUNCTION()
	void OnActionMouseR(const FInputActionValue& Value);
	UFUNCTION()
	void OnActionMouseL(const FInputActionValue& Value);

	void OnJump();
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE  USpringArmComponent* const  GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE  UCameraComponent* const GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE  UBuildingAssistComponent* const GetBuildingAssist() const { return BuildAssistComponent; }

	UFUNCTION(BlueprintPure, Category = "PlayerAnimation")
	FORCEINLINE  UPlayerAnimationComponent* const GetPlayerAnimationComponent() const { return PlayerAnimationComponent; }
	
	FORCEINLINE  float* GetStatusRef(EStatusType StatusType) {
		return  StatusArray.IsValidIndex((uint8)StatusType) ? &StatusArray[(uint8)StatusType] : &StatusArray[0];
	} 

	UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
	void SetStatus(EStatusType StatusType, float Value);
	UFUNCTION(BlueprintPure, Category = "PlayerStatus")
	bool GetStatus(EStatusType StatusType, float& Result);
};

