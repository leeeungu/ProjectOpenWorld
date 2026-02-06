#pragma once

#include "CoreMinimal.h"
#include "Pal/Component/Work/PalWorkComponent.h"
#include "PalWorkComponent_Architecture.generated.h"

class ABaseBuilding;

UCLASS()
class PROJECTOPENWORLD_API UPalWorkComponent_Architecture : public UPalWorkComponent
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TWeakObjectPtr < ABaseBuilding> TargetBuilding{};
public:
	UPalWorkComponent_Architecture();
	virtual bool CanWork() override;
	virtual void WorkStart(const FPalCommand& Command) override;
	virtual void WorkEvent(const FPalCommand& Command) override;
	virtual void WorkEnd(const FPalCommand& Command) override;
	virtual void WorkCancel() override;
	
};
