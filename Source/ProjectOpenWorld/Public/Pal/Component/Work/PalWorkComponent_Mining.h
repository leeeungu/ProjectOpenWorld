#pragma once

#include "CoreMinimal.h"
#include "Pal/Component/Work/PalWorkComponent.h"
#include "PalWorkComponent_Mining.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UPalWorkComponent_Mining : public UPalWorkComponent
{
	GENERATED_BODY()
	
public:
	UPalWorkComponent_Mining();
	virtual void WorkStart(AActor* Target) override;
	virtual void WorkEnd(AActor* Target) override;
	virtual void WorkCancel() override;
};
