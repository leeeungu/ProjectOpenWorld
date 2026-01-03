#pragma once

#include "CoreMinimal.h"
#include "GameBase/MetaData/AMD_MontageChangeEvent.h"
#include "AMD_MovementModeChangeEvent.generated.h"

class UCharacterMovementComponent;

UCLASS()
class PROJECTOPENWORLD_API UAMD_MovementModeChangeEvent : public UAMD_MontageChangeEvent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditInstanceOnly, Category = "ChangeEvent")
	TEnumAsByte<EMovementMode> StartMovemntMode = EMovementMode::MOVE_Flying;
	UPROPERTY(EditInstanceOnly, Category = "ChangeEvent")
	uint8 StartCustomMovemntMode{};

	UPROPERTY(EditInstanceOnly, Category = "ChangeEvent")
	TEnumAsByte<EMovementMode> EndMovemntMode = EMovementMode::MOVE_Walking;
	UPROPERTY(EditInstanceOnly, Category = "ChangeEvent")
	uint8 EndCustomMovemntMode{};

	UCharacterMovementComponent* GetCharacterMovementComponent(UAnimInstance* Animinstance);
public:
	virtual void StartEvent(UAnimInstance* Animinstance) override;
	virtual void EndEvent(UAnimInstance* Animinstance) override;
};
