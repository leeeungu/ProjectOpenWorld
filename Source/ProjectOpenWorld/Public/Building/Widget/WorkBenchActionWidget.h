#pragma once

#include "CoreMinimal.h"
#include "Building/Widget/BaseBuildingAction.h"
#include "WorkBenchActionWidget.generated.h"

class UTextBlock;

UCLASS()
class PROJECTOPENWORLD_API UWorkBenchActionWidget : public UBaseBuildingAction
{
	GENERATED_BODY()
protected:
	UWorkBenchActionWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativePreConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "WorkingBench", meta = (BindWidget))
	TObjectPtr<UTextBlock> BenchTitleText{};
};
