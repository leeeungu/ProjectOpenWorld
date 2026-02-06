#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/Interface/StatusUpdateInterface.h"
#include "PlayerStatusProgress.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;
class UStatComponent;

enum class EStatusType : uint8;

UCLASS()
class PROJECTOPENWORLD_API UPlayerStatusProgress : public UUserWidget, public IStatusUpdateInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr< UProgressBar> StatusProgress{};
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr< UTextBlock> StatusText{};
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr< UTextBlock> MaxStatusText{};

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "PlayerStatus", meta = (BindWidgetOptional))
	TObjectPtr< UImage> StatusImage{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PlayerStatus")
	FLinearColor ProgressColor{};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PlayerStatus")
	TObjectPtr < UTexture2D> StatusTexture{};

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "PlayerStatus")
	EStatusType StatusType{};

	TObjectPtr< UStatComponent> StatComponent{};
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void UpdateStatusData(double PreCurrentValue , double PreMaxValue) ;
public:
	//void SetStatusProgress(float* Value, float* MaxValue);

	// IStatusUpdateInterface을(를) 통해 상속됨
	UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
	void UpdateStatus() override;

	UFUNCTION(BlueprintPure, Category = "PlayerStatus")
	FORCEINLINE FText GetStatusText() const;
	UFUNCTION(BlueprintPure, Category = "PlayerStatus")
	FORCEINLINE FText GetMaxStatusText() const;

	UFUNCTION(BlueprintPure, Category = "PlayerStatus")
	FORCEINLINE float GetStatusPercent() const;
};
		