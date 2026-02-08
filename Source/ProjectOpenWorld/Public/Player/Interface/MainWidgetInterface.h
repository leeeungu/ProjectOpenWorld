#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MainWidgetInterface.generated.h"

class UUserWidget;

UINTERFACE(MinimalAPI)
class UMainWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IMainWidgetInterface
{
	GENERATED_BODY()
public:

public:
	virtual bool SetMainWidget() = 0;
	virtual void UnSetMainWidget() = 0;
};
