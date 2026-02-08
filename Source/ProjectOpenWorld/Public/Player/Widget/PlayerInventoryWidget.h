#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/Interface/MainWidgetInterface.h"
#include "PlayerInventoryWidget.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UPlayerInventoryWidget : public UUserWidget, public IMainWidgetInterface
{
	GENERATED_BODY()

public:
	// IMainWidgetInterface interface
	virtual bool SetMainWidget() override;
	virtual void UnSetMainWidget() override;
};
