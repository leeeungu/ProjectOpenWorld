#pragma once

#include "CoreMinimal.h"
#include "GameBase/BaseCharacter.h"
#include "GameBase/Interface/AttackInterface.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Interface/PalCommandInterface.h"
#include "Resource/Interface/ResourceInterface.h"
#include "Building/Interface/ArchitectureInterface.h"
#include "BaseCreature.generated.h"

struct FAIRequestID;
namespace EPathFollowingResult { enum Type : int; }
class UPalCommandComponent;
class UStaticMeshComponent;
class UPalAttackComponent;
class IGenericTeamAgentInterface;
class UInteractionComponent;

UCLASS()
class PROJECTOPENWORLD_API ABaseCreature : public ABaseCharacter, public IAttackInterface, 
	public IPalCommandInterface,
	public IResourceInterface, public IArchitectureInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UStaticMeshComponent> ArchitectureMeshComponent{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr < UPalCommandComponent> CommandComponent{};

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr < UPalAttackComponent> AttackComponent{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction)
	TObjectPtr<UInteractionComponent> InteractionComponent{};


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	float Hp{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	float Attack{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	float Defend{};	
	bool bActionStarted{};

protected:
	virtual void BeginPlay() override;
public:

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Damaged")
	FOnDamageedDelegate OnDamagedDelegate{};
public:
	ABaseCreature();
	virtual bool ReceiveCommand_Implementation(FPalCommand Command) override;
	virtual bool CommandPause_Implementation(bool bPause) override;


	UFUNCTION(BlueprintPure, Category = "CreatureAction")
	bool GetActionStarted() const { return bActionStarted; }

	UFUNCTION(BlueprintPure, Category = "CreatureAction")
	FORCEINLINE UPalAttackComponent* GetAttackComponent() const { return AttackComponent; }
	UFUNCTION(BlueprintPure, Category = "CreatureAction")
	FORCEINLINE UStaticMeshComponent* GetArchitectureMeshComponent() const { return ArchitectureMeshComponent; }

	UFUNCTION(BlueprintPure, Category = "CreatureAction")
	UPalCommandComponent* GetCommandComponent() const;
	float GetDefaultSpeed();
	void SetActionStarted(bool bValue);

	virtual void PossessedBy(AController* NewController) override;
public:
	//AttackInterface
	virtual float GetAttackValue_Implementation() const override;
	virtual void  SetAttackValue_Implementation(float NewValue) override;
	virtual void  RetAttackValue_Implementation() override;
	virtual bool DamagedCharacter_Implementation(const TScriptInterface< IAttackInterface>& Other) override;
	virtual bool IsDead_Implementation() const override;


	FORCEINLINE UInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }


	virtual float GetArchitectSpeed_Implementation() const override;
	virtual void StartArchitect_Implementation(ABaseBuilding* Building) override;
	virtual void StopArchitect_Implementation(ABaseBuilding* Building) override;
	virtual void EndArchitect_Implementation(ABaseBuilding* Building) override;

	virtual float GetResourceSpeed_Implementation() const override;
	virtual void StartResource_Implementation(AResourceActor* ResourceActor) override;
	virtual void StopResource_Implementation(AResourceActor* ResourceActor) override;
	virtual void EndResource_Implementation(AResourceActor* ResourceActor) override;
};
