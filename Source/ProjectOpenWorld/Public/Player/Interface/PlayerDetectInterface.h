#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerDetectInterface.generated.h"

class ABasePlayer;

UINTERFACE(MinimalAPI)
class UPlayerDetectInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IPlayerDetectInterface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintNativeEvent, Category = "PlayerDetect")
	void OnDetectBeginEvent(ABasePlayer* Player);
	UFUNCTION(BlueprintNativeEvent, Category = "PlayerDetect")
	void OnDetectEndEvent(ABasePlayer* Player);

};
