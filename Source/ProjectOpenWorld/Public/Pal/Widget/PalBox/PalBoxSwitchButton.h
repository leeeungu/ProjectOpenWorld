#pragma once

#include "CoreMinimal.h"
#include "Pal/Widget/BaseTabButtonWidget.h"
#include "PalBoxSwitchButton.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class PROJECTOPENWORLD_API UPalBoxSwitchButton : public UBaseTabButtonWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget), Category = "SwitchButton ")
	TObjectPtr<UImage> ButtonImage{};
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget), Category = "SwitchButton ")
	TObjectPtr<UTextBlock> ButtonNameText{};

	UPROPERTY(EditAnywhere, Category = "SwitchButton ")
	TObjectPtr<UTexture2D> Image{};
	UPROPERTY(EditAnywhere, Category = "SwitchButton ")
	FText NameText{};

	virtual void NativePreConstruct() override;
};
