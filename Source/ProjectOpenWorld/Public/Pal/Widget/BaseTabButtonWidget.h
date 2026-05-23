#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseTabButtonWidget.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UBaseTabButtonWidget : public UUserWidget
{
	GENERATED_BODY()
public:
#if WITH_EDITOR
	virtual void OnSelectedByDesigner() override;
#endif
protected:
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
private:
	bool ClickEvent();
};
