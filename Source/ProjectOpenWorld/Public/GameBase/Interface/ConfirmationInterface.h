#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ConfirmationInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UConfirmationInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IConfirmationInterface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Confirmation")
	void ConfirmationEvent(bool IsConfirmation);
};
