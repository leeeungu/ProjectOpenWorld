#pragma once

#include "CoreMinimal.h"
#include "Pal/Character/PalBaseCharacter.h"
#include "GameBase/Interface/AttackInterface.h"
#include "Pal/Interface/PalWorkerInterface.h"
#include "PalBaseCreature.generated.h"

class UPalPatrolComponent;
class UPalCommandComponent;
class UPalAttackComponent;
class UPalHitHandlerComponent;
class UStatComponent;
class UStaticMeshComponent;

UCLASS(Category = "PalCreature")
class PROJECTOPENWORLD_API APalBaseCreature : public APalBaseCharacter, public IAttackInterface, public IPalWorkerInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "PalCreature")
	FName PalName{};
	UPROPERTY(EditAnywhere, Category = "PalCreature")
	int Level{};

	// attack관련 componnet 공격 타입 관리 공격 중 관리
	UPROPERTY(VisibleAnywhere, Category = "PalCreature")
	TObjectPtr < UPalAttackComponent> AttackComponent{};

	// hit 관련 component
	UPROPERTY(VisibleAnywhere, Category = "PalCreature")
	TObjectPtr < UPalHitHandlerComponent> HitHandlerComponent{};

	UPROPERTY(VisibleAnywhere, Category = "PalCreature")
	TObjectPtr < UPalJobComponent> JobComponent{};

	UPROPERTY(VisibleAnywhere, Category = "PalCreature")
	TObjectPtr< UStaticMeshComponent> JobToolComponent{};
public:
	APalBaseCreature();

	TObjectPtr<UPalPatrolComponent> PalPatrolComponent{};
public:
	UFUNCTION(BlueprintPure, Category = "Pal|Component")
	FORCEINLINE UPalAttackComponent* GetAttackComponent() const { return AttackComponent; }
	UFUNCTION(BlueprintPure, Category = "Pal|Component")
	FORCEINLINE UPalJobComponent* GetJobComponent() const { return JobComponent; }
	UFUNCTION(BlueprintPure, Category = "Pal|Component")
	FORCEINLINE UStaticMeshComponent* GetPalToolMeshComponent() const { return JobToolComponent; }
	
	UFUNCTION(BlueprintPure, Category = "Pal|Component")
	int GetPalLevel() const { return Level; }
	virtual FName GetPalName() const override { return PalName; }

	// iattackinterface
	virtual float GetAttackValue_Implementation() const override;
	virtual float GetAttackDistance() const override;
	virtual bool IsDead_Implementation() const override;
	virtual void  SetAttackValue_Implementation(float NewValue) override {}
	virtual void  RetAttackValue_Implementation() override {}
	virtual bool DamagedCharacter_Implementation(const TScriptInterface< IAttackInterface>& Other) override { return true; }
	virtual UPalHitHandlerComponent* GetHitHandlerComponent() const override { return HitHandlerComponent; }
	virtual UPalAttackComponent* GetPalAttackComponent() const { return AttackComponent; }
	// IPalWorkerInterface
	virtual void StartWorking() override;
	virtual void StopWorking() override;
	virtual void EndWorking(bool bSuccess) override;
	virtual float GetWorkSpeed(EPalJobType JobType) override;
	virtual UPalJobComponent* GetPalJobComponent() const { return JobComponent; }

protected:
	void SetTransportWorkMoveSpeed(float MaxMoveSpeed);
	UFUNCTION()
	void OnStartTransport();
	UFUNCTION()
	void OnEndTransport();
	UFUNCTION()
	void OnWorkMeshChanged(UStaticMesh* NewMesh, FName SocketName, FTransform SocketTransform);
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnHPChanged(double PreCurrentStat, double CurrentStat);
};