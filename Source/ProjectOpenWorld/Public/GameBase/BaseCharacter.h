#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameBase/Interface/AttackInterface.h"
#include "BaseCharacter.generated.h"

class UStatComponent;

class UCharacterMontageComponent;

UCLASS(
	Abstract,                  // SpawnActor / NewObject 불가
	NotBlueprintable,          // BP 자식 클래스 못 만듦
	NotPlaceable,              // 레벨 드래그 배치 못함
	HideDropdown,              // 클래스 드롭다운에서 숨김
	meta = (
		DeprecationMessage = "Use APalBaseCharacter instead. This class will be removed."
		)
)
class  PROJECTOPENWORLD_API ABaseCharacter : public ACharacter, public IAttackInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr< UStatComponent> HPStat{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr< UStatComponent> AttackStat{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr< UStatComponent> DefendStat{};


	bool bDead{};
public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void UseControllerDesiredRotation();
	void UseOrientRotationToMovement();


	virtual bool LaunchAttack_Implementation(const TScriptInterface< IAttackInterface>& Other, FVector LaunchVelocity, bool bXYOverride, bool bZOverride);

	UFUNCTION(BlueprintPure, Category = "Status")
	UStatComponent* GetHPStat() const { return HPStat; }
	UFUNCTION(BlueprintPure, Category = "Status")
	UStatComponent* GetAttackStat() const { return AttackStat; }
	UFUNCTION(BlueprintPure, Category = "Status")
	UStatComponent* GetDefendStat() const { return DefendStat; }
};
