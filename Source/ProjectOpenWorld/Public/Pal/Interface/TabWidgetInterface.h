#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TabWidgetInterface.generated.h"

class UPanelWidget;
class UWidget;

UINTERFACE(MinimalAPI)
class UTabWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class ITabWidgetInterface
{
	GENERATED_BODY()
public:
	virtual void SetSelectedPanel(int32 NewSelectedIndex) {}
};
