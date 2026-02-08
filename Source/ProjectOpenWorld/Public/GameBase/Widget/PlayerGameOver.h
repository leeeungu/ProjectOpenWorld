#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/Interface/MainWidgetInterface.h"
#include "PlayerGameOver.generated.h"

class UButton;

UCLASS()
class PROJECTOPENWORLD_API UPlayerGameOver : public UUserWidget, public IMainWidgetInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RestartButton;

	UFUNCTION()
	void OnRestartButtonClicked();

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// 	IMainWidgetInterface interface
	virtual bool SetMainWidget() override;
	virtual void UnSetMainWidget() override;
};
