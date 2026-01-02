#pragma once

#include "CoreMinimal.h"
#include "GameBase/Animation/BaseAnimInstance.h"
#include "Pal/Data/PalCommandData.h"
#include "PalAnimInstance.generated.h"

class ABaseCreature;
class UPalCommandComponent;

UCLASS()
class PROJECTOPENWORLD_API UPalAnimInstance : public UBaseAnimInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, Category = "PalAnim")
	TObjectPtr < ABaseCreature> OwnerPalCreature{};
	UPROPERTY(BlueprintReadOnly, Category = "PalAnim")
	TObjectPtr<UPalCommandComponent> CommandComponent{};

	UPROPERTY(BlueprintReadWrite, Category = "PalAnim")
	EPalCommandKind CurrentCommandKind{};
	UPROPERTY(BlueprintReadWrite, Category = "PalAnim")
	uint8 SubCommandType{};
	UPROPERTY(BlueprintReadWrite, Category = "PalAnim")
	bool bActionStarted{};

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
};
