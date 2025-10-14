#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemInteractionToolTipWidget.generated.h"

class UTextBlock;

UCLASS()
class PROJECTOPENWORLD_API UItemInteractionToolTipWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemData", meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemNameTextBlock{};
public:
	virtual bool Initialize() override;
	void SetItemName(FName Name);
};
