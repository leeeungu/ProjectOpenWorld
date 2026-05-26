#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/Interface/StatusUpdateInterface.h"
#include "PlayerStatusProgress.generated.h"

class UStatusTextBlock;
class UImage;
class UStatusBarWidget;
enum class EStatusType : uint8;
class UStatComponent;

UCLASS()
class PROJECTOPENWORLD_API UPlayerStatusProgress : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr< UStatusTextBlock> StatusText{};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr < UStatusBarWidget> StatusBarWidget{};

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr< UImage> StatusImage{};
	UPROPERTY(EditAnywhere, Category = "PlayerStatus")
	TObjectPtr < UTexture2D> StatusTexture{};
	UPROPERTY(EditAnywhere, Category = "PlayerStatus", BlueprintReadOnly)
	FLinearColor ProgressColor{};
	UPROPERTY(EditAnywhere, Category = "PlayerStatus")
	EStatusType StatusType{};

	TWeakObjectPtr< UStatComponent> PlayerStatCom{};
public:
	EStatusType GetStatusType() const { return StatusType; }
	void SetStatWidget(UStatComponent* StatCom);
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void BindStat();
	void UnBindStat();
};
		