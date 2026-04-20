#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerPreviewWidget.generated.h"

class UImage;

UCLASS()
class PROJECTOPENWORLD_API UPlayerPreviewWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Preview", meta = (BindWidget))
	TObjectPtr<UImage> PlayerPreviewImage{};

	
};
