#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Pal/Data/PalCommandData.h"
#include "PalCommandInterface.generated.h"

UINTERFACE()
class UPalCommandInterface : public UInterface
{
	GENERATED_BODY()
};

class AActor;

class PROJECTOPENWORLD_API IPalCommandInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ReceiveCommand(FPalCommand Command);
};
