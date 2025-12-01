#pragma once

#include "CoreMinimal.h"
#include "Pal/Component/PalCommandComponent.h"
#include "PalAllyCommandComponent.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UPalAllyCommandComponent : public UPalCommandComponent
{
	GENERATED_BODY()

public:
	UPalAllyCommandComponent()  = default;

	virtual void OnStartCurrentCommand() override;
	virtual void OnEndCurrentCommand() override;
};
