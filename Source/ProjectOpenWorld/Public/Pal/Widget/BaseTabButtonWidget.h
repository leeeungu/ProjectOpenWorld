#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseTabButtonWidget.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UBaseTabButtonWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Tab")
	int32 TabIndex{ INDEX_NONE };
public:
#if WITH_EDITOR
	virtual void OnSelectedByDesigner() override;
#endif
protected:
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
private:
	bool ClickEvent();
	int32 ResolveTabIndex() const;
};
