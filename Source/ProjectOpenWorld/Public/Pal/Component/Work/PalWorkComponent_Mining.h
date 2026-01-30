#pragma once

#include "CoreMinimal.h"
#include "Pal/Component/Work/PalWorkComponent.h"
#include "PalWorkComponent_Mining.generated.h"

class UInteractionComponent;

UCLASS()
class PROJECTOPENWORLD_API UPalWorkComponent_Mining : public UPalWorkComponent
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TObjectPtr<UInteractionComponent> InteractionComp{};
public:
	UPalWorkComponent_Mining();
protected:
	virtual void BeginPlay() override;
public:
	virtual bool CanWork() override;
	virtual void WorkStart(const FPalCommand& Command) override;
	virtual void WorkEvent(const FPalCommand& Command) override;
	virtual void WorkEnd(const FPalCommand& Command) override;
	virtual void WorkCancel() override;
};
