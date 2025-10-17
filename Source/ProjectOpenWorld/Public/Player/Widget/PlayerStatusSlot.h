#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/Interface/StatusUpdateInterface.h"
#include "PlayerStatusSlot.generated.h"

class UTextBlock;
class UImage;


UCLASS()
class PROJECTOPENWORLD_API UPlayerStatusSlot : public UUserWidget, public IStatusUpdateInterface
{
	GENERATED_BODY()
protected:
	float* StatusRef{};
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr< UTextBlock> StatusText{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr< UTextBlock> StatusNameText{};
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "PlayerStatus", meta = (BindWidget))
	TObjectPtr< UImage> StatusImage{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PlayerStatus")
	FText StatusName{};
protected:
	virtual void NativePreConstruct() override;
public:
	void SetStatusSlot(float* Value) { StatusRef = Value; }

	// IStatusUpdateInterface을(를) 통해 상속됨
	void UpdateStatus() override;
};
