#pragma once

#include "CoreMinimal.h"
#include "GameBase/Animation/BaseAnimInstance.h"
#include "PalCreatureAnimInstance.generated.h"

class APalBaseCreature;
class UPalJobComponent;
enum class EPalJobType : uint8;

UCLASS()
class PROJECTOPENWORLD_API UPalCreatureAnimInstance : public UBaseAnimInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, Category = "PalAnim")
	TObjectPtr <APalBaseCreature> OwnerPalCreature{};
	UPROPERTY(VisibleAnywhere, Category = "PalAnim")
	TObjectPtr<UPalJobComponent> JobComponent{};

	UPROPERTY(EditAnywhere, Category = "PalAnim")
	EPalJobType CurrenJobType{};
	UPROPERTY(EditAnywhere, Category = "PalAnim")
	bool bActionStarted{};

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, Category = "PalAnim", meta = (BlueprintThreadSafe))
	EPalJobType GetCurrentJobType() const { return CurrenJobType; }
	UFUNCTION(BlueprintPure, Category = "PalAnim", meta = (BlueprintThreadSafe))
	bool IsActionStarted() const { return bActionStarted; }
};
