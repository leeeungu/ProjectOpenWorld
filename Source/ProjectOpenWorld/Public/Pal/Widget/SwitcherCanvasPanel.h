#pragma once

#include "CoreMinimal.h"
#include "Components/CanvasPanel.h"
#include "Pal/Interface/TabWidgetInterface.h"
#include "SwitcherCanvasPanel.generated.h"

class UWidgetSwitcher;

UCLASS()
class PROJECTOPENWORLD_API USwitcherCanvasPanel : public UCanvasPanel, public ITabWidgetInterface
{
	GENERATED_BODY()
public:
	// 전환 대상 스위처. BP(PreConstruct)에서 지정.
	UPROPERTY(EditAnywhere, Category = "Tab")
	TObjectPtr<UWidgetSwitcher> TargetSwitcher{};

	UPROPERTY(EditAnywhere, Category = "Tab")
	int32 DefaultIndex{};

	virtual void SetSelectedPanel(int32 Index) override;

protected:
	//virtual void OnWidgetRebuilt() override;
	virtual void SynchronizeProperties() override;
};
