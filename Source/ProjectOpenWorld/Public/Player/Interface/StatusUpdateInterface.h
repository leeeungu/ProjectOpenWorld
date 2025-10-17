#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StatusUpdateInterface.generated.h"

UINTERFACE(MinimalAPI)
class UStatusUpdateInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IStatusUpdateInterface
{
	GENERATED_BODY()
public:

	virtual void UpdateStatus() = 0;
};
