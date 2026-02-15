#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OpenWorldGameModeBase.generated.h"

UCLASS()
class PROJECTOPENWORLD_API AOpenWorldGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void StartPlay() override;
};
