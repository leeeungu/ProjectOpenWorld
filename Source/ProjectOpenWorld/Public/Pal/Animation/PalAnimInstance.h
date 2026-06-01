#pragma once

#include "CoreMinimal.h"
#include "GameBase/Animation/BaseAnimInstance.h"
#include "Pal/Data/PalCommandData.h"
#include "PalAnimInstance.generated.h"

class ABaseCreature;
class UPalCommandComponent;

UCLASS(
	Abstract,                  // SpawnActor / NewObject 불가
	NotBlueprintable,          // BP 자식 클래스 못 만듦
	NotPlaceable,              // 레벨 드래그 배치 못함
	HideDropdown,              // 클래스 드롭다운에서 숨김
	meta = (
		DeprecationMessage = "Use UPalCreatureAnimInstance instead. This class will be removed."
		)
)
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
