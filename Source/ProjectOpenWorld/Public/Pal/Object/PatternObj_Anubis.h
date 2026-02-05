#pragma once

#include "CoreMinimal.h"
#include "Pal/Object/PatternObj.h"
#include "PatternObj_Anubis.generated.h"

class ABossMonster_Anubis;
class ABasePlayer;

UENUM(BlueprintType)
enum class EAnubisPatternType : uint8
{
	Anubis_Pattern_01 UMETA(DisplayName = "Anubis_Pattern_01"),
};


UCLASS()
class PROJECTOPENWORLD_API UPatternObj_Anubis01 : public UPatternObj
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PatternData")
	float PatternMoveSpeed = 500.0f;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "PatternData")
	FVector PatternMoveDirection{};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PatternData")
	float  PatternRotateSpeedYaw = 720.0f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PatternData")
	int PatternSolveCount = 3;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PatternData")
	float PatternStopTime = 1;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PatternData")
	float StunDuration = 2.f;

	int CurrentCount{};
	float CurTime{};
protected:
	TObjectPtr<ABossMonster_Anubis> OwnerCharacter{};
	TObjectPtr<ABasePlayer> TargetPlayer{};
public:
	UPatternObj_Anubis01() = default;

	virtual void InitializePattern(AActor* OwnerActor, AActor* TargetActor) override;
	virtual void StartPattern() override;
	virtual void UpdatePattern(float DeltaTime) override;
	virtual void EndPattern() override;
	virtual void UpdateCondition() override;
};