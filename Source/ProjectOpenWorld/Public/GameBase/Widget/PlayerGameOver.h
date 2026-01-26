#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerGameOver.generated.h"

class UButton;

UCLASS()
class PROJECTOPENWORLD_API UPlayerGameOver : public UUserWidget
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

};
