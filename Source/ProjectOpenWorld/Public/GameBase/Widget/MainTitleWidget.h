#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainTitleWidget.generated.h"

class UButton;
enum EUISoundType : uint8;

UCLASS()
class PROJECTOPENWORLD_API UMainTitleWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> StartButton{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> ExitButton{};

	UPROPERTY(EditAnywhere, Category= "Title")
	TEnumAsByte<EUISoundType> ButtonClickSoundType{}; 
	UPROPERTY(EditAnywhere, Category= "Title")
	TEnumAsByte<EUISoundType> ButtonHoverSoundType{}; 
	UPROPERTY(EditAnywhere, Category= "Title")
	TSoftObjectPtr<UWorld> MainLevel{};

protected:
	UFUNCTION()
	void OnStartButtonClicked();
	UFUNCTION()
	void OnExitButtonClicked();
	UFUNCTION()
	void OnButtonHovered();

public:
	virtual void NativeOnInitialized() override;

	
};
