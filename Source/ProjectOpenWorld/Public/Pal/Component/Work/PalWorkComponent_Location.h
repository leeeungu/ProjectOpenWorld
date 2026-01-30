#pragma once

#include "CoreMinimal.h"
#include "Pal/Component/Work/PalWorkComponent.h"
#include "PalWorkComponent_Location.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UPalWorkComponent_Location : public UPalWorkComponent
{
	GENERATED_BODY()
public:
	UPalWorkComponent_Location();
protected:
	virtual void BeginPlay() override;
public:
	virtual bool CanWork() override;
	virtual void WorkStart(const FPalCommand& Command) override;
	virtual void WorkEvent(const FPalCommand& Command) override;
	virtual void WorkEnd(const FPalCommand& Command) override;
	virtual void WorkCancel() override;
};
