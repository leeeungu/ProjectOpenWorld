#pragma once

#include "CoreMinimal.h"
#include "Creature/Component/CreatureActionComponent.h"
#include "CreatureAction_Attack.generated.h"

class APawn;
struct FAIRequestID;
namespace EPathFollowingResult { enum Type : int; }

UENUM(Blueprintable)
enum class ECreatureAttackIndex : uint8
{
	CreatureAttackIndex_None,
	CreatureAttackIndex_Default,
	CreatureAttackIndex_Skill01,
	CreatureAttackIndex_Max UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FCreatureAttackData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "AttackData")
	float AttackDelay{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "AttackData")
	float AttackDamage = 1;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttackStart, ECreatureAttackIndex, Index);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UCreatureAction_Attack : public UCreatureActionComponent
{
	GENERATED_BODY()
private:
	TSoftObjectPtr<APawn> TargetPawn{};

	//TSoftObjectPtr < UCreatureAttackComponent> AttackComponent{};

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	TMap<ECreatureAttackIndex, FCreatureAttackData> MapAttackData{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Attack")
	ECreatureAttackIndex CurAttackIndex{};
	const FCreatureAttackData* CurAttackData{};

	TSoftObjectPtr < ACharacter> OwnerCharacter{};
	TArray < ECreatureAttackIndex> ArrayAttackIndex{};

public:
	UPROPERTY(BlueprintAssignable, Category = "Attack")
	FAttackStart StartDelegate{};
public:
	UCreatureAction_Attack();

	virtual void ActionStart_Implementation(ECreatureActionType ActionType, UObject* TargetObject) override;
	virtual void ActionEnd_Implementation() override;


	UFUNCTION(BlueprintPure, Category = "Attack")
	ECreatureAttackIndex GetAttackIndex() const { return CurAttackIndex; }

	UFUNCTION(BlueprintPure, Category = "Attack")
	bool GetAttackDamage(float& fDamage) const;

	UFUNCTION(BlueprintPure, Category = "Attack")
	bool GetAttackDelay(float& fTime) const;
protected:
	virtual void BeginPlay() override;

	void SetRandomIndex();
	UFUNCTION()
	void MoveToTarget();
	UFUNCTION()
	void FinishMoved(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};
