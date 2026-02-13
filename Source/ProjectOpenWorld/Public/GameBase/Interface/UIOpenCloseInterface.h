#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UIOpenCloseInterface.generated.h"

UINTERFACE(MinimalAPI)
class UUIOpenCloseInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IUIOpenCloseInterface
{
	GENERATED_BODY()

public:
	virtual void OpenUI() = 0;
	virtual void CloseUI() = 0;
};
