#pragma once

#include "CoreMinimal.h"
#include "Pal/Component/Work/PalWorkComponent.h"
#include "PalWorkComponent_Mining.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UPalWorkComponent_Mining : public UPalWorkComponent
{
	GENERATED_BODY()
private:
	bool bIsWorking{};
	
public:
	UPalWorkComponent_Mining();
	virtual void WorkStart(const FPalCommand& Command) override;
	virtual void WorkEnd(const FPalCommand& Command) override;
	virtual void WorkCancel() override;
};
