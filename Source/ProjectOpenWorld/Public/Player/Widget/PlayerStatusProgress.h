#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/Interface/StatusUpdateInterface.h"
#include "PlayerStatusProgress.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class PROJECTOPENWORLD_API UPlayerStatusProgress : public UUserWidget, public IStatusUpdateInterface
{
	GENERATED_BODY()
protected:
	float* StatusRef{};
	float* MaxStatusRef{};
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr< UProgressBar> StatusProgress{};
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr< UTextBlock> StatusText{};
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr< UTextBlock> MaxStatusText{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PlayerStatus")
	FLinearColor ProgressColor{};
protected:
	virtual void NativePreConstruct() override;
public:
	void SetStatusProgress(float* Value, float* MaxValue);

	// IStatusUpdateInterface을(를) 통해 상속됨
	void UpdateStatus() override;

	UFUNCTION(BlueprintPure, Category = "PlayerStatus")
	FORCEINLINE float GetStatusPercent() const;
};
	