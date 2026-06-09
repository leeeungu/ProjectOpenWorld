#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatusSlot.generated.h"

class UTextBlock;
class UImage;
class UTexture2D;
enum class EStatusType : uint8;
class UStatComponent;

UCLASS()
class PROJECTOPENWORLD_API UPlayerStatusSlot : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr< UTextBlock> StatusText{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr< UTextBlock> StatusNameText{};
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr< UImage> StatusImage{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PlayerStatus")
	FText StatusName{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PlayerStatus")
	EStatusType StatusType{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PlayerStatus")
	TObjectPtr < UTexture2D> StatusTexture{};
protected:
	virtual void NativePreConstruct() override;
public:
	void SetStatWidget(UStatComponent* StatCom);
private:
	UFUNCTION()
	void OnMaxStatusChanged(double PreMaxStat, double InMaxStat);
};
