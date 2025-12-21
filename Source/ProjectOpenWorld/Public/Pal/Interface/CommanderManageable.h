#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Pal/Data/PalCommandData.h"
#include "CommanderManageable.generated.h"

UINTERFACE(MinimalAPI)
class UCommanderManageable : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API ICommanderManageable
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CommanderManageable")
	EPalCommandKind GetCommandKind();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CommanderManageable")
	uint8 GetSubCommandType();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CommanderManageable")
	FPalCommand GetCommand();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CommanderManageable")
	bool IsCommandFinished();
};
